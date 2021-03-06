#ifndef TEST_NVM_ADAPTER_H_
#define TEST_NVM_ADAPTER_H_

#include <NVMInterface.h>
#include <WString.h>

class testNVM : public NVMInterface
{
public:
    testNVM() : NVMInterface()
    {
    }

    ~testNVM()
    {
    }

    uint8_t begin()
    {
        return 0;
    }

    uint8_t end()
    {
        return 0;
    }

    void save(const String &name, int32_t value)
    {
        m_outputInt = value;
    }

    void save(const String &name, const String &value)
    {
        m_outputString = value;
    }

    uint32_t readInt(String name)
    {
        String str;
        int counter = 0;
        uint32_t out = 0;

        while (name[counter] != 0)
        {
            str += name[counter];
            counter++;
        }

        if (str == "AutoStart")
        {
            out = m_autostart;
        }

        if (str == "ToggleTime")
        {
            out = m_toggletime;
        }

        return out;
    }

    String readString(String name)
    {
        String str;
        String out;
        int counter = 0;

        while (name[counter] != 0)
        {
            str += name[counter];
            counter++;
        }

        if (str == "SerialBaud")
        {
            out = "U5";
        }
        else if (str == "CanBaud")
        {
            out = "S6";
        }
        else if (str == "FilterMode")
        {
            out = "W0";
        }
        else if (str == "FilterACn")
        {
            out = "M00000000";
        }
        else if (str == "FilterAMn")
        {
            out = "mFFFFFFFF";
        }

        return out;
    }

    String m_outputString;
    uint32_t m_outputInt;
    uint32_t m_toggletime = 0;
    uint32_t m_autostart = 0;

private:
};

#endif