#include <string>
#include <cstring>
#include <cstdio>
#include "solver.h"
#include "maneuver.h"
#include <iostream>

#define _DESC_      "A Rubik's Cube Solver"
#define _VERSION_   "0.1"
#define _AUTHOR_    "Coshz"

bool is_valid_cube(const char *cube);

struct REPL
{
    REPL(const char* prog)
    {
        printf("Welcome to %s!\n"
               "[Desc] %s\n"
               "[Usage] %s\n", prog, _DESC_, "`:h` for help, `:q` for quit");
    }

    void run();
    std::string help();

    /* -1: unsupported command; -2: invaid argument; n: the num of parsed arguments */
    int parse(char *line);

    /* used to store parsed arguments */
    int cmd; char *args[5];

    /* all supported commands */
    static constexpr const char *cmdset[] = { "solve", "info", ":h", ":q" };
};

std::string REPL::help()
{
    std::string h = 
    "[Help]\n"
    "    solve N [from] [to=id]     -- solve the [cube] in N steps if possible\n"
    "                                  (default N:20)\n"
    "    info  L [maneuver]         -- show the [maneuver] in level L\n"
    "                                  (0:color cube; 1:cubie decomposition)\n"
    "where:\n"
    "   [cube] is the color configuration of cube;  eg: UUUUUUUUURRRRRRRRRFFFFFFFFFDDDDDDDDDLLLLLLLLLBBBBBBBBB\n"
    "   [maneuver] is the moves acting on cube;     eg: FRL'B2D\n"
    ;
    return h;
}

int REPL::parse(char *line)
{
    static const char *delimiters = " ";
    static const int max_n_args = 5;

    // clear states
    cmd = -1; for(int i = 0; i < max_n_args; i++) args[i] = NULL;

    int i = 0, k = 0; // cmd_id, argument_cnt
    char *token = strtok(line, delimiters);
    
    // check command
    while(i < 4 && strcmp(token, cmdset[i])) i++;
    if(i >= 4) return -1;
    else cmd = i;

    // check arguments
    switch(cmd)
    {
    case 0: // `solve N cube_src cube_tgt`
        while(k < max_n_args) {
            token = strtok(NULL, " ");
            if(token) args[k++] = token; else break;
        }
        if((k == 0) ||
           (k == 1 && !is_valid_cube(args[0])) ||
           (k == 2 && ((!std::atoi(args[0]) && !is_valid_cube(args[0])) || !is_valid_cube(args[1]))) ||
           (k >= 3 && !std::atoi(args[0]) &&  !is_valid_cube(args[1]) && !is_valid_cube(args[2]))
        ) return -2;
        break;
    case 1: // `info N "maneuver"`
        token = strtok(NULL, " "), args[0] = token;
        while(token && *token == ' ') token++; //skip spaces after args[0]
        if(token) token = strtok(token, "\""), args[1] = token;
        if(!args[0] || !args[1]) return -2;
        // k = sscanf(token, "%s \"%s\"", args[0], args[1]);
        // if((k <= 1) 
        //    // || (k > 1 && !std::atoi(args[0])) // 0 is allowed
        // ) return -2;
        break;
    default: ;
    }
    return k;
}

void REPL::run()
{
    size_t no=0;
    static char line[120];
    std::string res;
    while(true)
    {
        printf("\nIn [%zu]:= ", no);
        scanf(" %[(': \")a-zA-Z0-9]", line), getchar();
    
        if(!strcmp(line, "")){ continue; }
        int rc = parse(line);

        if(cmd == 3) { break; }
        if (rc == -1) { puts("[Error] not supported command.\n"); continue; }
        else if (rc == -2) { puts("[Error] invalid Arguments\n"); continue; }

        int N = (args[0]) ? std::atoi(args[0]) : 0;
        switch(cmd)
        {
        case 0: 
            if(rc == 1) res = CubeSolver::solve(args[0]);
            else if (rc == 2) res = (N > 0) ? CubeSolver::solve(args[1], "", N)
                                            : CubeSolver::solve(args[0],args[1]);
            else res = CubeSolver::solve(args[1], args[2], N);
            break;
        case 1: 
            res = Maneuver(args[1]).str(N); 
            break;
        case 2: puts(help().c_str()); continue;
        }
        printf("\nOut[%zu]: %s\n", no++, res.c_str());
    }
    printf("\nGoodbye!\n");
}

/* first running will take seconds to generate tables before main() */
int main(int argc, char *argv[])
{
    // std::atexit(doCleanupTables);
    auto repl = REPL(argv[0]);
    repl.run();
}