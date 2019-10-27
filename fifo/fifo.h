/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   fifo.h
 * Author: ignas
 *
 * Created on May 14, 2017, 2:10 PM
 */

#ifndef FIFO_H
#define FIFO_H

#include <vector>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <stdio.h>
#include <string.h>
#include <chrono>

template <class T> class PacketFIFO
{
public:
    PacketFIFO(int size);
    ~PacketFIFO();
    int Push(T* &data);
    T* Pop(int timeout_ms = 10);
    
    const int size;
    std::atomic<int> itemCount;
private:
    std::vector<T*> data;
    T *popVal;
    unsigned rdInd;
    unsigned wrInd;  
    std::mutex m;
    std::condition_variable cv;
};


template <class T> PacketFIFO<T>::PacketFIFO(int cnt): size(cnt)
{
    rdInd = wrInd = 0;
    data.resize(size);
    popVal = new T;
    for (int i = 0; i < size; i++)
        data[i] = new T;
}
template <class T> PacketFIFO<T>::~PacketFIFO()
{
    for (unsigned i = 0; i < size; i++)
        delete data[i];     
    delete popVal;
}
template <class T> int PacketFIFO<T>::Push(T* &pkt)
{ 
    if (itemCount.load() < size) 
    {
        auto tmp = data[wrInd];
        data[wrInd] = pkt;
        pkt = tmp;
        if (++wrInd == size)
            wrInd = 0;
        itemCount++;
        cv.notify_one();
        return 0;
    }
    return -1;
}
template <class T> T* PacketFIFO<T>::Pop(int timeout_ms)
{
    std::unique_lock<std::mutex> lk(m);
    
    if (itemCount.load() == 0)
        if (cv.wait_for(lk, std::chrono::milliseconds(timeout_ms), [this]{return (itemCount.load() > 0);})==false)
            return nullptr;
    
    auto tmp = data[rdInd];
    data[rdInd] = popVal;
    popVal = tmp;
    if (++rdInd == size)
        rdInd = 0;
    itemCount--;
    return popVal;
}
#endif /* FIFO_H */

