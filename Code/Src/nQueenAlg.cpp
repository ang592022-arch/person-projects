/*****************************************************
 * Course: CS111  | Term: 2026 Spring
 * Instructor: zyliang@must.edu.mo
 * Homework Assignment: 3
 *****************************************************/


/*****************************************************
 * If you modified this file, provide the following information: 
 * GROUP INFORMATION (FILL IN ACCURATELY)
王子昂 D2
 * Notes on Modifications (optional): 
 * [Briefly describe the changes you made to this file]
 *****************************************************/

#include "../Include/nQueenAlg.hpp"

// !!!!!!!!!!!!!! < Hmk3 Task 6 > !!!!!!!!!!!!!!
// Provide the missing code of this file. You can design 
// this class. Some code outside this class is also possible

#include <iostream>
#include <cstring>

namespace CS111
{
namespace BOARD_GAME
{
    using namespace std;

    bool find_one_sol(char** board, int n, Seq& sol)
    {
        int r = sol.size();
        if(r == n)
            return true;

        for(int c = 0; c < n; ++c)
        {
            if(board[r][c] == 'Q')
            {
                bool conflict = false;
                for(int i = 0; i < r; ++i)
                {
                    int cc = sol[i];
                    if(cc == c || abs(i - r) == abs(cc - c))
                    {
                        conflict = true;
                        break;
                    }
                }
                if(conflict)
                    return false;
                sol.push_back(c);
                bool ret = find_one_sol(board,n,sol);
                if(ret) return true;
                sol.pop_back();
                return false;
            }
            bool conflict = false;
            for(int i=0;i<n;i++)
                for(int j=0;j<n;j++)
                    if(board[i][j]=='Q' && (j==c||abs(i-r)==abs(j-c)))
                        conflict=true;
            if(conflict) continue;
            for(int i=0;i<r;i++)
            {
                int cc=sol[i];
                if(cc==c||abs(i-r)==abs(cc-c)){conflict=true;break;}
            }
            if(conflict) continue;

            char old=board[r][c];
            board[r][c]='Q';
            sol.push_back(c);
            bool res=find_one_sol(board,n,sol);
            if(res) return true;
            sol.pop_back();
            board[r][c]=old;
        }
        return false;
    }

    bool find_all_sols(char** board, int n, Seq& sol, SeqSeq& sols)
    {
        int r=sol.size();
        if(r==n)
        {
            sols.push_back(sol);
            return true;
        }
        bool ok=false;
        for(int c=0;c<n;c++)
        {
            if(board[r][c]=='Q')
            {
                bool conf=false;
                for(int i=0;i<r;i++)
                {
                    int cc=sol[i];
                    if(cc==c||abs(i-r)==abs(cc-c)){conf=true;break;}
                }
                if(conf) return false;
                sol.push_back(c);
                bool sub=find_all_sols(board,n,sol,sols);
                ok|=sub;
                sol.pop_back();
                return ok;
            }
            bool conf=false;
            for(int i=0;i<n;i++)
                for(int j=0;j<n;j++)
                    if(board[i][j]=='Q'&&(j==c||abs(i-r)==abs(j-c))) conf=true;
            if(conf) continue;
            for(int i=0;i<r;i++)
            {
                int cc=sol[i];
                if(cc==c||abs(i-r)==abs(cc-c)){conf=true;break;}
            }
            if(conf) continue;

            char old=board[r][c];
            board[r][c]='Q';
            sol.push_back(c);
            bool sub=find_all_sols(board,n,sol,sols);
            ok|=sub;
            sol.pop_back();
            board[r][c]=old;
        }
        return ok;
    }

    NQueenAlg::NQueenAlg(int size)
    : NQueen(size, "NQueenAlg", std::vector<std::string>{"PlayerA","PlayerB"})
{
}

    void NQueenAlg::print_one_solution()
    {
        int n=size_;
        char** brd=new char*[n];
        for(int i=0;i<n;i++)
        {
            brd[i]=new char[n];
            memset(brd[i],' ',n);
        }
        Seq res;
        find_one_sol(brd,n,res);
        for(int i=0;i<res.size();i++) brd[i][res[i]]='Q';
        for(int i=0;i<n;i++)
        {
            for(int j=0;j<n;j++) cout<<brd[i][j]<<" ";
            cout<<endl;
        }
        for(int i=0;i<n;i++) delete[] brd[i];
        delete[] brd;
    }

    void NQueenAlg::print_all_solutions()
    {
        int n=size_;
        char** brd=new char*[n];
        for(int i=0;i<n;i++)
        {
            brd[i]=new char[n];
            memset(brd[i],' ',n);
        }
        Seq tmp;
        SeqSeq all;
        find_all_sols(brd,n,tmp,all);
        cout<<"Total = "<<all.size()<<endl;
        for(auto& arr:all)
        {
            for(int i=0;i<n;i++) memset(brd[i],' ',n);
            for(int i=0;i<arr.size();i++) brd[i][arr[i]]='Q';
            for(int i=0;i<n;i++)
            {
                for(int j=0;j<n;j++) cout<<brd[i][j]<<" ";
                cout<<endl;
            }
            cout<<"--------\n";
        }
        for(int i=0;i<n;i++) delete[] brd[i];
        delete[] brd;
    }
}
}
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
