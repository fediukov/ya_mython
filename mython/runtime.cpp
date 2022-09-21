#include "runtime.h"

#include <cassert>
#include <iostream>
#include <optional>
#include <sstream>

using namespace std;

namespace runtime {

    const std::string STR_METHOD = "__str__"s;
    const std::string EQ_METHOD = "__eq__"s;
    const std::string LT_METHOD = "__lt__"s;


    // ------------------------------------------------------------------------ //
    // ObjectHolder ----------------------------------------------------------- //
    ObjectHolder::ObjectHolder(std::shared_ptr<Object> data)
        : data_(std::move(data)) {
    }

    void ObjectHolder::AssertIsValid() const {
        assert(data_ != nullptr);
    }

    ObjectHolder ObjectHolder::Share(Object& object) {
        // ���������� ����������� shared_ptr (��� deleter ������ �� ������)
        return ObjectHolder(std::shared_ptr<Object>(&object, [](auto* /*p*/) { /* do nothing */ }));
    }

    ObjectHolder ObjectHolder::None() {
        return ObjectHolder();
    }

    Object& ObjectHolder::operator*() const {
        AssertIsValid();
        return *Get();
    }

    Object* ObjectHolder::operator->() const {
        AssertIsValid();
        return Get();
    }

    Object* ObjectHolder::Get() const {
        return data_.get();
    }

    ObjectHolder::operator bool() const {
        return Get() != nullptr;
    }

    // ------------------------------------------------------------------------ //
    // IsTrue ----------------------------------------------------------------- //
    bool IsTrue(const ObjectHolder& object) {
        if ((object.TryAs<String>() != nullptr && object.TryAs<String>()->GetValue() != "")
            || (object.TryAs<Number>() != nullptr && object.TryAs<Number>()->GetValue())
            || (object.TryAs<Bool>() != nullptr && object.TryAs<Bool>()->GetValue()))
        {
            return true;
        }
        return false;
    }
    // ------------------------------------------------------------------------ //
    // ClassInstance ---------------------------------------------------------- //
    ClassInstance::ClassInstance(const Class& cls)
        : cls_(cls)
    {
    }

    void ClassInstance::Print(std::ostream& os, [[maybe_unused]] Context& context)
    {
        if (HasMethod(STR_METHOD, 0))
        {
            Call(STR_METHOD, {}, context).TryAs<Object>()->Print(os, context);
        }
        else
        {
            os << this;
        }
    }

    bool ClassInstance::HasMethod(const std::string& method, size_t argument_count) const
    {
        if (cls_.GetMethod(method) && cls_.GetMethod(method)->formal_params.size() == argument_count)
        {
            return true;
        }
        return false;
    }

    Closure& ClassInstance::Fields()
    {
        return symbol_table_;
    }

    const Closure& ClassInstance::Fields() const
    {
        return symbol_table_;
    }


    ObjectHolder ClassInstance::Call(const std::string& method, 
        const std::vector<ObjectHolder>& actual_args,
        Context& context)
    {
        if (HasMethod(method, actual_args.size()))
        {
            Closure args;
            const Method* m = cls_.GetMethod(method);
            args.insert(std::make_pair("self", ObjectHolder::Share(*this)));
            for (size_t i{ 0 }; i < m->formal_params.size(); ++i)
            {
                args.insert(std::make_pair(m->formal_params.at(i), actual_args.at(i)));
            }
            return cls_.GetMethod(method)->body->Execute(args, context);
        }
        throw std::runtime_error("Not implemented"s);
    }

    // ------------------------------------------------------------------------ //
    // Class ------------------------------------------------------------------ //
    Class::Class(std::string name, std::vector<Method> methods, const Class* parent)
        : name_(std::move(name)), parent_(parent)
    {
        for (auto& m : methods)
        {
            Method* method = new Method(std::move(m));
            methods_.push_back(method);
        }//*/
    }

    const Method* Class::GetMethod(const std::string& name) const
    {
        for (const auto m : methods_)
        {
            if (m->name == name)
            {
                return m;
            }
        }
        if (parent_)
        {
            return parent_->GetMethod(name);
        }
        // ��������. ���������� ����� ��������������
        return nullptr;
    }

    [[nodiscard]] /*inline*/ const std::string& Class::GetName() const
    {
        return name_;
    }

    void Class::Print(ostream& os, [[maybe_unused]] Context& context)
    {
        os << "Class " << name_;
    }

    // ------------------------------------------------------------------------ //
    void Bool::Print(std::ostream& os, [[maybe_unused]] Context& context) {
        os << (GetValue() ? "True"sv : "False"sv);
    }

    // ------------------------------------------------------------------------ //
    // Comparing -------------------------------------------------------------- //
    bool Equal(const ObjectHolder& lhs, const ObjectHolder& rhs, [[maybe_unused]] Context& context)
    {
        if (lhs.TryAs<String>() != nullptr && rhs.TryAs<String>() != nullptr)
        {
            return lhs.TryAs<String>()->GetValue() == rhs.TryAs<String>()->GetValue();
        }
        else if (lhs.TryAs<Number>() != nullptr && rhs.TryAs<Number>() != nullptr)
        {
            return lhs.TryAs<Number>()->GetValue() == rhs.TryAs<Number>()->GetValue();
        }
        else if (lhs.TryAs<Bool>() != nullptr && rhs.TryAs<Bool>() != nullptr)
        {
            return lhs.TryAs<Bool>()->GetValue() == rhs.TryAs<Bool>()->GetValue();
        }
        else if (lhs.TryAs<ClassInstance>() != nullptr && rhs.TryAs<ClassInstance>() != nullptr)
        {
            return lhs.TryAs<ClassInstance>()->Call(EQ_METHOD, {rhs}, context).TryAs<Bool>()->GetValue();
        }
        else if ((lhs.TryAs<String>() == nullptr && rhs.TryAs<String>() == nullptr)
            && (lhs.TryAs<Number>() == nullptr && rhs.TryAs<Number>() == nullptr)
            && (lhs.TryAs<Bool>() == nullptr && rhs.TryAs<Bool>() == nullptr)
            && (lhs.TryAs<ClassInstance>() == nullptr && rhs.TryAs<ClassInstance>() == nullptr))
        {
            return true;
        }
        else
        {
            throw std::runtime_error("Cannot compare objects for equality"s);
        }
    }

    bool Less(const ObjectHolder& lhs, const ObjectHolder& rhs, [[maybe_unused]] Context& context)
    {
        if (lhs.TryAs<String>() != nullptr && rhs.TryAs<String>() != nullptr)
        {
            return lhs.TryAs<String>()->GetValue() < rhs.TryAs<String>()->GetValue();
        }
        if (lhs.TryAs<Number>() != nullptr && rhs.TryAs<Number>() != nullptr)
        {
            return lhs.TryAs<Number>()->GetValue() < rhs.TryAs<Number>()->GetValue();
        }
        if (lhs.TryAs<Bool>() != nullptr && rhs.TryAs<Bool>() != nullptr)
        {
            return lhs.TryAs<Bool>()->GetValue() < rhs.TryAs<Bool>()->GetValue();
        }
        if (lhs.TryAs<ClassInstance>() != nullptr && rhs.TryAs<ClassInstance>() != nullptr)
        {
            return lhs.TryAs<ClassInstance>()->Call(LT_METHOD, { rhs }, context).TryAs<Bool>()->GetValue();
        }
        throw std::runtime_error("Cannot compare objects for equality"s);
    }

    bool NotEqual(const ObjectHolder& lhs, const ObjectHolder& rhs, [[maybe_unused]] Context& context)
    {
        return !(Equal(lhs, rhs, context));
    }

    bool Greater(const ObjectHolder& lhs, const ObjectHolder& rhs, [[maybe_unused]] Context& context)
    {
        return !Less(lhs, rhs, context) && !Equal(lhs, rhs, context);
    }

    bool LessOrEqual(const ObjectHolder& lhs, const ObjectHolder& rhs, [[maybe_unused]] Context& context)
    {
        return Less(lhs, rhs, context) || Equal(lhs, rhs, context);
    }

    bool GreaterOrEqual(const ObjectHolder& lhs, const ObjectHolder& rhs, [[maybe_unused]] Context& context)
    {
        return !Less(lhs, rhs, context);
    }

}  // namespace runtime