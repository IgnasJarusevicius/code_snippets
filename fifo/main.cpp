/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: ignas
 *
 * Created on May 14, 2017, 2:09 PM
 */

#include <cstdlib>
#include "fifo.h"
#include <iostream>
#include <thread>
#include <thread>
using namespace std;

auto fifo = new PacketFIFO<int>(1024*4);
/*
 * 
 */
void Producer()
{
    int* pValue = new int; 
    for (int i = 0; i<1024*1024; i++)
    {
        *pValue = i;
        while (fifo->Push(pValue)!=0)
        {
            std::cout << "FIFO full" << endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
    delete pValue;
}

int main(int argc, char** argv) 
{
    
    std::thread t1(Producer);
   
    for (int i = 0; ; i++)
    {
        int *popedVal = fifo->Pop();
        if (popedVal)
        {
            //cout << *popedVal << endl;
        }
        else
        {
            cout << "NULL" << endl;
            break;
        }
    }
    t1.join(); 
    delete fifo;
    return 0;
}

