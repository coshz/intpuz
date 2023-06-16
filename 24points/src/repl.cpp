#include <iostream>
#include <string>
#include <filesystem>
#include <sstream>
#include <vector>
#include <any>
#include "24points.h"

std::string __version__ = "0.1.1";
std::string __author__ = "Coshz";
std::string __desc__ = "A commandline tool to solve 24-point";


struct REPL 
{
    REPL(const std::string &prog)
    {
        std::cout << "Welcome to " << prog << "!"
                  << "\n;[Desc] " << __desc__
                  << "\n;[Usage] :? for help, :q for quit.\n";
    }
    std::string help()
    {
        std::string h = "\n;[Help]";
        h += "\n\tSolve[{x1,...,xn},tgt=24]             -- non-greedy";
        h += "\n\tSolve2[{x1,...,xn},tgt=24]            -- greedy";
        h += "\n\ttgt x1 x2 ... xn                      -- shortcut";
        h += "\n; Here: 4 <= n <= 8";
        h += "\n";
        return h;
    }
    void run()
    {
        std::string line;
        int no=0;
        help();
		auto pre = [](const int &no){
			std::cout << "\nIn[" << no << "]:= ";
		};
        while(pre(no), getline(std::cin, line))
        {
			if(line == "") continue;
			else if(line == ":?") { std::cout << help(); no++; }
            else if(line == ":q") { std::cout << "\nGoodbye.\n"; exit(0); }
            else try{
                std::cout << "Out[" << no << "]= ";
                execute(line);
				no++;
            } catch(std::exception& e) {
                std:: cout << e.what() << std::endl;
            }
        }
    }
    void execute(const std::string &query)
    {
        vector<int> v;
        int tgt;
        parse(query, v, tgt);

        static Model<4> model4;
        static Model<5> model5;
        static Model<6> model6;
        static Model<7> model7;
        static Model<8> model8;
        
        decltype(model4.solve(std::declval<vector<int>>(), std::declval<int>())) exprs;

        switch(v.size()) {
            case 4: exprs = model4.solve(v,tgt); break;
            case 5: exprs = model5.solve(v,tgt); break;
            case 6: exprs = model6.solve(v,tgt); break;
            case 7: exprs = model7.solve(v,tgt); break;
            case 8: exprs = model8.solve(v,tgt); break;
            default: std::cerr << "the size is not supported now.\n";
        }

        if(exprs.size() == 0) {
            std::cout << "No Solution.\n";
        } else {
            for(auto &expr: exprs) std::cout << expr.to_str() << std::endl;
        }
    }

    void parse(const std::string &line, vector<int>&v, int& tgt)
    {
        int n;
        std::stringstream ss(line);
        ss >> tgt;
        v.clear();
        while(ss >> n) { v.push_back(n); }
    }
};

int main(int argc, char* argv[])
{
    auto prog = std::filesystem::path(argv[0]).filename();
    auto repl = REPL(std::string(prog));
    repl.run();
}
