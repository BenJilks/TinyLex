#pragma once
#include <string>
#include <iostream>
#include <fstream>
using std::string;

namespace TinyScript
{

	struct Token
	{
		enum TokenType
		{
			TOKEN_TEST,
		};

		static const int count = 1;
		string data;
		TokenType type;
	};

    class Lexer
    {
    public:
        Lexer(std::istream &stream);
        Lexer(string file_path);
        Token next();

        inline bool is_eof() const { return eof_flag; }

    private:
        enum class MatchStatus
        {
            MATCH,
            BLANK,
            ERROR
        };
        MatchStatus next_match(Token &token);

        std::istream &in;
        std::ifstream file_in;
        char back_log;
        bool eof_flag;

    };

#ifdef TINYLEX_IMPLEMENT

	static int TEST_end_states[] = {4, 2, };
	static char TEST[] = 
	{
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	};

	static char *type_table[] = {TEST, };

	static int *type_end_states[] = {TEST_end_states, };

	static int type_end_state_count[] = {2, };

    Lexer::Lexer(std::istream &stream) :
        in(stream), back_log('\0'), eof_flag(false) {}

    Lexer::Lexer(string file_path) :
        in(file_in), back_log('\0'), eof_flag(false)
    {
        file_in = std::ifstream(file_path);
    }

    Lexer::MatchStatus Lexer::next_match(Token &token)
    {
        int state = 0;
        int buffer_pointer = 0;
        bool not_blank = false;

        for (int i = 0; i < Token::count; i++)
        {
            state = 0;
            buffer_pointer = 0;
            do
            {
                if (buffer_pointer >= token.data.size())
                {
                    char next = in.get();
                    if (next == EOF)
                        eof_flag = true;
                    token.data += next;
                }

                char c = token.data[buffer_pointer++];
                state = type_table[i][state * 128 + c];
                back_log = c;

                for (int j = 0; j < type_end_state_count[i]; j++)
                {
                    if (type_end_states[i][j] == state)
                    {
                        token.type = (Token::TokenType)i;
                        return Lexer::MatchStatus::MATCH;
                    }
                }
                
                if (buffer_pointer > 1)
                    not_blank = true;
            } while(state != -1 && !eof_flag);
        }

        return not_blank ? 
            Lexer::MatchStatus::ERROR : 
            Lexer::MatchStatus::BLANK;
    }

    Token Lexer::next()
    {
        string buffer = "";
        if (back_log != '\0')
            buffer.push_back(back_log);

        Lexer::MatchStatus status;
        Token token;
        do
        {
            if (token.data.length() > 0)
                token.data = token.data.substr(1, token.data.length() - 1);
            status = next_match(token);

            if (status == Lexer::MatchStatus::ERROR)
                printf("Error: Unknown token '%s'\n", token.data.c_str());
        } while (status != Lexer::MatchStatus::MATCH && !eof_flag);

        buffer.pop_back();
        return token;
    }

#endif

}
