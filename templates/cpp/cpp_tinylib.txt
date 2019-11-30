#ifndef TINY_LIB
#define TINY_LIB
#include <vector>
#include <string>
#include <iostream>

namespace TinyLib
{

    class TinyError
    {
    public:
        TinyError() :
            error_flag(false) {}

        bool has_errors() const
        {
            // If this has an error, then return that there is one
            if (error_flag) 
                return true;

            // Search its dependencies for errors
            for (TinyError *dep : deps)
                if (dep->has_errors())
                    return true;

            // Otherwise there is no errors to report
            return false;
        }

        virtual void report_errors(std::ostream &out = std::cout)
        {
            for (TinyError *dep : deps)
                dep->report_errors(out);
        }

    
    protected:
        void add_dependency(TinyError *dep) { deps.push_back(dep); }
        bool error_flag;

    private:
        std::vector<TinyError*> deps;

    };

    template<typename Token, typename TokenType>
    class LexerInterface : public TinyError
    {
    public:
        LexerInterface() {}
        ~LexerInterface() {}

        virtual Token next() = 0;
        virtual Token match(TokenType type, std::string name) = 0;
        virtual const Token &get_look() const = 0;
        virtual bool is_eof() const = 0;
    };

}

#endif
