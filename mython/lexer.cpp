#include "lexer.h"

#include <algorithm>
#include <charconv>
#include <unordered_map>

using namespace std;

namespace parse {

    bool operator==(const Token& lhs, const Token& rhs) {
        using namespace token_type;

        if (lhs.index() != rhs.index()) {
            return false;
        }
        if (lhs.Is<Char>()) {
            return lhs.As<Char>().value == rhs.As<Char>().value;
        }
        if (lhs.Is<Number>()) {
            return lhs.As<Number>().value == rhs.As<Number>().value;
        }
        if (lhs.Is<String>()) {
            return lhs.As<String>().value == rhs.As<String>().value;
        }
        if (lhs.Is<Id>()) {
            return lhs.As<Id>().value == rhs.As<Id>().value;
        }
        return true;
    }

    bool operator!=(const Token& lhs, const Token& rhs) {
        return !(lhs == rhs);
    }

    std::ostream& operator<<(std::ostream& os, const Token& rhs) {
        using namespace token_type;

#define VALUED_OUTPUT(type) \
    if (auto p = rhs.TryAs<type>()) return os << #type << '{' << p->value << '}';

        VALUED_OUTPUT(Number);
        VALUED_OUTPUT(Id);
        VALUED_OUTPUT(String);
        VALUED_OUTPUT(Char);

#undef VALUED_OUTPUT

#define UNVALUED_OUTPUT(type) \
    if (rhs.Is<type>()) return os << #type;

        UNVALUED_OUTPUT(Class);
        UNVALUED_OUTPUT(Return);
        UNVALUED_OUTPUT(If);
        UNVALUED_OUTPUT(Else);
        UNVALUED_OUTPUT(Def);
        UNVALUED_OUTPUT(Newline);
        UNVALUED_OUTPUT(Print);
        UNVALUED_OUTPUT(Indent);
        UNVALUED_OUTPUT(Dedent);
        UNVALUED_OUTPUT(And);
        UNVALUED_OUTPUT(Or);
        UNVALUED_OUTPUT(Not);
        UNVALUED_OUTPUT(Eq);
        UNVALUED_OUTPUT(NotEq);
        UNVALUED_OUTPUT(LessOrEq);
        UNVALUED_OUTPUT(GreaterOrEq);
        UNVALUED_OUTPUT(None);
        UNVALUED_OUTPUT(True);
        UNVALUED_OUTPUT(False);
        UNVALUED_OUTPUT(Eof);

#undef UNVALUED_OUTPUT

        return os << "Unknown token :("sv;
    }

    Lexer::Lexer(std::istream& input)
    {
        it_ = std::istreambuf_iterator<char>(input);
        ParseLexeme();
    }

    /*const Token& Lexer::CurrentToken() const {
        // Çàãëóøêà. Ðåàëèçóéòå ìåòîä ñàìîñòîÿòåëüíî
        throw std::logic_error("Not implemented"s);
    }//*/

    /*Token Lexer::NextToken() {
        // Çàãëóøêà. Ðåàëèçóéòå ìåòîä ñàìîñòîÿòåëüíî
        throw std::logic_error("Not implemented"s);
    }//*/

    void Lexer::ParseLexeme()
    {
        auto it = it_;
        auto end = end_;
        while (true)
        {
            // parse end of file (including indents and new_line)
            if (it == end)
            {
                AddEofLexemå();

                it_ = it;
                break;
            }
        
            std::string s = "";
            // parse indents/dedents
            if (tokens_.empty() || tokens_.back().Is<token_type::Newline>())
            {
                while ((*it) == ' ')
                {
                    s.push_back(*it);
                    ++it;
                }
                // checking is not ignored line except before eof
                if (((*it) != '\n' && (*it) != '#') || it == end)
                {
                    // checkig and add indent/dedent
                    if (s.size() % 2)
                    {
                        throw;
                    }
                    if (AddIndentLexeme(s.size()))
                    {
                        it_ = it;
                        break;
                    }
                }
                s = "";
            }

            // ignoring spaces in the middle of the line
            if ((*it) == ' ')
            {
                ++it;
            }
            // parse word lexeme
            else if ((*it) == '_' || ((*it) >= 65 && (*it) <= 90) || ((*it) >= 97 && (*it) <= 122))
            {
                while ((*it) == '_' || ((*it) >= 65 && (*it) <= 90) || ((*it) >= 97 && (*it) <= 122) || ((*it) >= 48 && (*it) <= 57))
                {
                    s.push_back(*it);
                    ++it;
                    if (it == end) { break; }
                }

                AddWordLexemå(s);

                it_ = it;
                break;
            }
            // parse number lexeme
            else if ((*it) >= 48 && (*it) <= 57)
            {
                while ((*it) >= 48 && (*it) <= 57)
                {
                    s += (*it);
                    ++it;
                    if (it == end) { break; }
                }

                AddNumberLexemå(s);

                it_ = it;
                break;
            }
            // parse char lexeme
            else if ((*it) == '-' || (*it) == '+' || (*it) == '*' || (*it) == '/'
                || (*it) == ':' || (*it) == '(' || (*it) == ')' || (*it) == ',' || (*it) == '.')
            {
                AddCharLexemå(*it);
                ++it;

                it_ = it;
                break;
            }
            // parse other operator lexeme
            else if ((*it) == '<' || (*it) == '>' || (*it) == '!' || (*it) == '=')
            {
                char c = (*it);
                ++it;
                if (it == end) { AddCharLexemå(c); AddEofLexemå(); break; }
                if (ComparingLexeme(c, (*it)))
                {
                    ++it;
                }

                it_ = it;
                break;
            }
            // parse string lexeme
            else if ((*it) == '\'' || (*it) == '"')
            {
                char c = (*it);
                ++it;
                if (it == end) { AddEofLexemå(); break; }
                while ((*it) != c)
                {
                    if ((*it) == '\\')
                    {
                        ++it;
                        if (it == end) { AddEofLexemå(); break; }
                        if ((*it) == '\"')
                        {
                            s += '\"';
                        }
                        else if ((*it) == '\'')
                        {
                            s += '\'';
                        }
                        else if ((*it) == 't')
                        {
                            s += '\t';
                        }
                        else if ((*it) == 'n')
                        {
                            s += '\n';
                        }//*/
                        ++it;
                    }
                    else
                    {
                        s += (*it);
                        ++it;
                        if (it == end) { break; }
                    }
                }
                ++it;

                AddStringLexemå(s);

                it_ = it;
                break;
            }
            // parse comment lexeme
            else if ((*it) == '#')
            {
                while (true)
                {
                    ++it;
                    if (it == end || *it == '\n') { break; }
                }
            }
            // parse end of line
            else if ((*it) == '\n')
            {
                if (!tokens_.empty() && !tokens_.back().Is<token_type::Indent>() && !tokens_.back().Is<token_type::Newline>())
                {
                    AddNewLineLexemå();

                    it_ = it;
                    break;
                }
                ++it;
            }
        }
    }

    void Lexer::AddWordLexemå(const std::string& s)
    {
        if (s == "class")
        {
            token_type::Class token;
            tokens_.emplace_back(token);
        }
        else if (s == "return")
        {
            token_type::Return token;
            tokens_.emplace_back(token);
        }
        else if (s == "if")
        {
            token_type::If token;
            tokens_.emplace_back(token);
        }
        else if (s == "else")
        {
            token_type::Else token;
            tokens_.emplace_back(token);
        }
        else if (s == "def")
        {
            token_type::Def token;
            tokens_.emplace_back(token);
        }
        else if (s == "print")
        {
            token_type::Print token;
            tokens_.emplace_back(token);
        }
        else if (s == "or")
        {
            token_type::Or token;
            tokens_.emplace_back(token);
        }
        else if (s == "None")
        {
            token_type::None token;
            tokens_.emplace_back(token);
        }
        else if (s == "and")
        {
            token_type::And token;
            tokens_.emplace_back(token);
        }
        else if (s == "not")
        {
            token_type::Not token;
            tokens_.emplace_back(token);
        }
        else if (s == "True")
        {
            token_type::True token;
            tokens_.emplace_back(token);
        }
        else if (s == "False")
        {
            token_type::False token;
            tokens_.emplace_back(token);
        }
        else
        {
            token_type::Id token{ s };
            tokens_.emplace_back(token);
        }
    }

    void Lexer::AddCharLexemå(const char& c)
    {
        token_type::Char token{ c };
        tokens_.emplace_back(token);
    }

    void Lexer::AddNumberLexemå(const std::string& s)
    {
        token_type::Number token{ std::stoi(s) };
        tokens_.emplace_back(token);
    }

    void Lexer::AddStringLexemå(const std::string& s)
    {
        token_type::String token{ s };
        tokens_.emplace_back(token);
    }

    bool Lexer::ComparingLexeme(const char& c, const char& next_c)
    {
        if (c == '=' && next_c == '=')
        {
            AddEqLexemå();
        }
        else if (c == '!' && next_c == '=')
        {
            AddNotEqLexemå();
        }
        else if (c == '!' && next_c != '=')
        {
            return false;
        }
        else if (c == '<' && next_c == '=')
        {
            AddLessOrEqLexemå();
        }
        else if (c == '>' && next_c == '=')
        {
            AddGreaterOrEqLexemå();
        }
        else
        {
            AddCharLexemå(c);
            return false;
        }
        return true;
    }

    void Lexer::AddEqLexemå()
    {
        token_type::Eq token;
        tokens_.emplace_back(token);
    }

    void Lexer::AddNotEqLexemå()
    {
        token_type::NotEq token;
        tokens_.emplace_back(token);
    }

    void Lexer::AddLessOrEqLexemå()
    {
        token_type::LessOrEq token;
        tokens_.emplace_back(token);
    }

    void Lexer::AddGreaterOrEqLexemå()
    {
        token_type::GreaterOrEq token;
        tokens_.emplace_back(token);
    }

    bool Lexer::AddIndentLexeme(size_t indent)
    {
        indent /= 2;
        token_type::Indent indent_token;
        token_type::Dedent dedent_token;
        if (indent == indent_)
        {
            return false;
        }
        else if (indent > indent_)
        {
            while (indent != indent_)
            {
                tokens_.emplace_back(indent_token);
                ++indent_;
            }
        }
        else if (indent < indent_)
        {
            while (indent != indent_)
            {
                tokens_.emplace_back(dedent_token);
                --indent_;
            }
        }

        return true;
    }

    void Lexer::AddNewLineLexemå()
    {
        token_type::Newline token;
        tokens_.emplace_back(token);
    }

    void Lexer::AddEofLexemå()
    {
        if (!tokens_.empty() && !tokens_.back().Is<token_type::Newline>())
        {
            AddNewLineLexemå();
        }

        if (indent_ > 0)
        {
            AddIndentLexeme(0);
        }

        token_type::Eof token;
        tokens_.emplace_back(token);
    }

}  // namespace parse