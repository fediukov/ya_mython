#pragma once

#include <iosfwd>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

namespace parse {

    namespace token_type {

        struct Number { int value; };           // 0 Лексема «число» // число 
        struct Id { std::string value; };       // 1 Лексема «идентификатор» // Имя идентификатора
        struct Char { char value; };            // 2 Лексема «символ» // код символа 
        struct String { std::string value; };   // 3 Лексема «строковая константа»
        struct Class {};                        // 4 Лексема «class»
        struct Return {};                       // 5 Лексема «return»
        struct If {};                           // 6 Лексема «if»
        struct Else {};                         // 7 Лексема «else»
        struct Def {};                          // 8 Лексема «def»
        struct Newline {};                      // 9 Лексема «конец строки»
        struct Print {};                        // 10 Лексема «print»
        struct Indent {};                       // 11 Лексема «увеличение отступа», соответствует двум пробелам
        struct Dedent {};                       // 12 Лексема «уменьшение отступа»
        struct And {};                          // 13 Лексема «and»
        struct Or {};                           // 14 Лексема «or»
        struct Not {};                          // 15 Лексема «not»
        struct Eq {};                           // 16 Лексема «==»
        struct NotEq {};                        // 17 Лексема «!=»
        struct LessOrEq {};                     // 18 Лексема «<=»
        struct GreaterOrEq {};                  // 19 Лексема «>=»
        struct None {};                         // 20 Лексема «None»
        struct True {};                         // 21 Лексема «True»
        struct False {};                        // 22 Лексема «False»
        struct Eof {};                          // 23 Лексема «конец файла»

    }  // namespace token_type

    using TokenBase
        = std::variant<token_type::Number, token_type::Id, token_type::Char, token_type::String,
        token_type::Class, token_type::Return, token_type::If, token_type::Else,
        token_type::Def, token_type::Newline, token_type::Print, token_type::Indent,
        token_type::Dedent, token_type::And, token_type::Or, token_type::Not,
        token_type::Eq, token_type::NotEq, token_type::LessOrEq, token_type::GreaterOrEq,
        token_type::None, token_type::True, token_type::False, token_type::Eof>;

    struct Token : TokenBase {
        using TokenBase::TokenBase;

        template <typename T>
        [[nodiscard]] bool Is() const {
            return std::holds_alternative<T>(*this);
        }

        template <typename T>
        [[nodiscard]] const T& As() const {
            return std::get<T>(*this);
        }

        template <typename T>
        [[nodiscard]] const T* TryAs() const {
            return std::get_if<T>(this);
        }
    };

    bool operator==(const Token& lhs, const Token& rhs);
    bool operator!=(const Token& lhs, const Token& rhs);

    std::ostream& operator<<(std::ostream& os, const Token& rhs);

    class LexerError : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

    class Lexer {
    public:
        explicit Lexer(std::istream& input);

        // Возвращает ссылку на текущий токен или token_type::Eof, если поток токенов закончился
        [[nodiscard]] const Token& CurrentToken() const
        {
            if (tokens_.size())
            {
                return tokens_.at(current_);
            }
            else
            {
                throw;
            }
        }

        // Возвращает следующий токен, либо token_type::Eof, если поток токенов закончился
        Token NextToken()
        {
            if (tokens_.back() != token_type::Eof())
            {
                ParseLexeme();
            }

            if (tokens_.size())
            {
                if (current_ < tokens_.size() - 1)
                {
                    current_++;
                }
                return tokens_.at(current_);
            }
            else
            {
                throw;
            }
        }

        // Если текущий токен имеет тип T, метод возвращает ссылку на него.
        // В противном случае метод выбрасывает исключение LexerError
        template <typename T>
        const T& Expect() const {
            using namespace std::literals;
            if (tokens_.at(current_).Is<T>())
            {
                return tokens_.at(current_).As<T>();
            }
            // Заглушка. Реализуйте метод самостоятельно
            throw LexerError("Not implemented"s);
        }

        // Метод проверяет, что текущий токен имеет тип T, а сам токен содержит значение value.
        // В противном случае метод выбрасывает исключение LexerError
        template <typename T, typename U>
        void Expect(const U& value) const {
            using namespace std::literals;
            if (tokens_.at(current_).Is<T>())
            {
                if (tokens_.at(current_).As<T>().value == value)
                {
                    return;
                }
            }
            // Заглушка. Реализуйте метод самостоятельно
            throw LexerError("Not implemented"s);
        }

        // Если следующий токен имеет тип T, метод возвращает ссылку на него.
        // В противном случае метод выбрасывает исключение LexerError
        template <typename T>
        const T& ExpectNext() {
            using namespace std::literals;
            if (NextToken().Is<T>())
            {
                return tokens_.at(current_).As<T>();;
            }
            // Заглушка. Реализуйте метод самостоятельно
            throw LexerError("Not implemented"s);
        }

        // Метод проверяет, что следующий токен имеет тип T, а сам токен содержит значение value.
        // В противном случае метод выбрасывает исключение LexerError
        template <typename T, typename U>
        void ExpectNext(const U& value) {
            using namespace std::literals;
            if (NextToken().Is<T>())
            {
                if (tokens_.at(current_).As<T>().value == value)
                {
                    return;
                }
            }
            // Заглушка. Реализуйте метод самостоятельно
            throw LexerError("Not implemented"s);
        }

    private:
        // main parse method
        void ParseLexeme();

        //secondary parse methods
        void AddWordLexemе(const std::string& s);
        void AddCharLexemе(const char& c);
        void AddNumberLexemе(const std::string& s);
        void AddStringLexemе(const std::string& s);
        bool ComparingLexeme(const char& c1, const char& c2);
        void AddEqLexemе();
        void AddNotEqLexemе();
        void AddLessOrEqLexemе();
        void AddGreaterOrEqLexemе();
        bool AddIndentLexeme(size_t indent);
        void AddNewLineLexemе();
        void AddEofLexemе();

    private:
        std::vector<Token> tokens_;
        std::istreambuf_iterator<char> it_;
        std::istreambuf_iterator<char> end_ = std::istreambuf_iterator<char>();
        size_t current_ = 0;
        size_t indent_ = 0;
    };

}  // namespace parse