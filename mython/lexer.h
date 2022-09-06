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

        struct Number { int value; };           // 0 ������� ������ // ����� 
        struct Id { std::string value; };       // 1 ������� �������������� // ��� ��������������
        struct Char { char value; };            // 2 ������� ������� // ��� ������� 
        struct String { std::string value; };   // 3 ������� ���������� ���������
        struct Class {};                        // 4 ������� �class�
        struct Return {};                       // 5 ������� �return�
        struct If {};                           // 6 ������� �if�
        struct Else {};                         // 7 ������� �else�
        struct Def {};                          // 8 ������� �def�
        struct Newline {};                      // 9 ������� ������ ������
        struct Print {};                        // 10 ������� �print�
        struct Indent {};                       // 11 ������� ����������� �������, ������������� ���� ��������
        struct Dedent {};                       // 12 ������� ����������� �������
        struct And {};                          // 13 ������� �and�
        struct Or {};                           // 14 ������� �or�
        struct Not {};                          // 15 ������� �not�
        struct Eq {};                           // 16 ������� �==�
        struct NotEq {};                        // 17 ������� �!=�
        struct LessOrEq {};                     // 18 ������� �<=�
        struct GreaterOrEq {};                  // 19 ������� �>=�
        struct None {};                         // 20 ������� �None�
        struct True {};                         // 21 ������� �True�
        struct False {};                        // 22 ������� �False�
        struct Eof {};                          // 23 ������� ������ �����

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

        // ���������� ������ �� ������� ����� ��� token_type::Eof, ���� ����� ������� ����������
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

        // ���������� ��������� �����, ���� token_type::Eof, ���� ����� ������� ����������
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

        // ���� ������� ����� ����� ��� T, ����� ���������� ������ �� ����.
        // � ��������� ������ ����� ����������� ���������� LexerError
        template <typename T>
        const T& Expect() const {
            using namespace std::literals;
            if (tokens_.at(current_).Is<T>())
            {
                return tokens_.at(current_).As<T>();
            }
            // ��������. ���������� ����� ��������������
            throw LexerError("Not implemented"s);
        }

        // ����� ���������, ��� ������� ����� ����� ��� T, � ��� ����� �������� �������� value.
        // � ��������� ������ ����� ����������� ���������� LexerError
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
            // ��������. ���������� ����� ��������������
            throw LexerError("Not implemented"s);
        }

        // ���� ��������� ����� ����� ��� T, ����� ���������� ������ �� ����.
        // � ��������� ������ ����� ����������� ���������� LexerError
        template <typename T>
        const T& ExpectNext() {
            using namespace std::literals;
            if (NextToken().Is<T>())
            {
                return tokens_.at(current_).As<T>();;
            }
            // ��������. ���������� ����� ��������������
            throw LexerError("Not implemented"s);
        }

        // ����� ���������, ��� ��������� ����� ����� ��� T, � ��� ����� �������� �������� value.
        // � ��������� ������ ����� ����������� ���������� LexerError
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
            // ��������. ���������� ����� ��������������
            throw LexerError("Not implemented"s);
        }

    private:
        // main parse method
        void ParseLexeme();

        //secondary parse methods
        void AddWordLexem�(const std::string& s);
        void AddCharLexem�(const char& c);
        void AddNumberLexem�(const std::string& s);
        void AddStringLexem�(const std::string& s);
        bool ComparingLexeme(const char& c1, const char& c2);
        void AddEqLexem�();
        void AddNotEqLexem�();
        void AddLessOrEqLexem�();
        void AddGreaterOrEqLexem�();
        bool AddIndentLexeme(size_t indent);
        void AddNewLineLexem�();
        void AddEofLexem�();

    private:
        std::vector<Token> tokens_;
        std::istreambuf_iterator<char> it_;
        std::istreambuf_iterator<char> end_ = std::istreambuf_iterator<char>();
        size_t current_ = 0;
        size_t indent_ = 0;
    };

}  // namespace parse