// (c) Abyss Group
// (c) NAWE Group

#if !defined(FIELD_H)
#define FIELD_H

class Field
{
public:

    inline void SetValue(char* value) { mValue = value; }

    inline const char *GetString() { return mValue; }
    inline float GetFloat() { return mValue ? static_cast<float>(atof(mValue)) : 0; }
    inline bool GetBool() { return mValue ? atoi(mValue) > 0 : false; }
    inline uint8 GetUInt8() { return mValue ? static_cast<uint8>(atol(mValue)) : 0; }
	inline int8 GetInt8() { return mValue ? static_cast<int8>(atol(mValue)) : 0; }
    inline uint16 GetUInt16() { return mValue ? static_cast<uint16>(atol(mValue)) : 0; }
    inline uint32 GetUInt32() { return mValue ? static_cast<uint32>(atol(mValue)) : 0; }
    uint64 GetUInt64() 
    {
        if(mValue)
        {
            uint64 value;
            sscanf(mValue,I64FMTD,&value);
            return value;
        }
        else
            return 0;
    }

private:
        char *mValue;
};

#endif
