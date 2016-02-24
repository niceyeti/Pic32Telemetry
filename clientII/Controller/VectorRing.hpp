#ifndef VECTOR_RING_HPP
#define VECTOR_RING_HPP

#include "../my_types.h"
#include "SysCommon.hpp"

/*
  TODO: none of the multithreading protections are well-defined.
  TODO: genericize andmake this a template class, casting it to <IMU>
*/

class VectorRing
{
  public:
    VectorRing() = delete;
    VectorRing(int maxItems);
    ~VectorRing();
    void Push(const IMU* inVec);
    int Begin();
    int End();
    bool GetAt(int i, IMU& outVec);
    void Peek(IMU& outVec); //gets latest vector
    bool HasNewData();
    void ClearNewData();
    static void CopyVec(const IMU& src,IMU& dest);

  private:
    volatile bool newData;
    void advance();
    vector<IMU> imuRing;
    int begin;
    int _begin; //always one ahead of begin
    int end;
    const int maxSize;
};

#endif
