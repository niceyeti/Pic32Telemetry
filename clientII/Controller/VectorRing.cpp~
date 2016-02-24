#include "VectorRing.hpp"

VectorRing::VectorRing(int maxItems) : maxSize(maxItems)
{
  imuRing.resize(maxItems);
  _begin = 0;
  begin = 0;
  end = 0;
}

bool VectorRing::HasNewData()
{
  return newData;
}

void VectorRing::ClearNewData()
{
  newData = false;
}

//doesn't belong here
void VectorRing::CopyVec(const IMU& src,IMU& dest)
{
  memcpy((void*)&dest,(void*)&src,sizeof(IMU));
}

bool VectorRing::GetAt(int i, IMU& outVec)
{
  if(i < maxSize){
    CopyVec(imuRing[i],outVec);
    return true;
  }
  return false;
}

void VectorRing::Peek(IMU& outVec)
{
  CopyVec(imuRing[begin],outVec);
}

void VectorRing::advance()
{
  //order of these updates matters: disallow readers from ever obtaining begin when begin==_begin
  _begin++;
  if(_begin == imuRing.size()){
    _begin = 0;
    begin = maxSize - 1;
  }
  else{
    begin = _begin - 1;
  }

  //discard old data
  if(end == begin){
    end++;
    if(end == imuRing.size()){
      end = 0;
    }
  }
}

//TODO: check if full
void VectorRing::Push(const IMU* inVec)
{
  //copy first, then advance
  memcpy((void*)&imuRing[_begin],(void*)inVec,sizeof(IMU));
  advance();
  newData = true;
}

int VectorRing::End()
{
  return end;
}

int VectorRing::Begin()
{
  return begin;
}

VectorRing::~VectorRing()
{
  imuRing.clear();
}
