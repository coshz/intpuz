#ifndef __OPTION_H__
#define __OPTION_H__
#include <string>

struct Option
{
    /* next-queue size: 0 ~ 6. */
    Option(const std::string &config);
    int next;
    bool hold;
    bool ghost;
    /* lockdown rulesets: 0 ~ 2 for infinite placement, extended, classic. */
    int lockDown;
    /* background music, sound effect. */
    bool mediea[];
};

#endif // __OPTION_H__