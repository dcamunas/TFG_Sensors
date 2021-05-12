#define MAX_VOLTAGE 400
#define MIN_VOLTAGE 0

class iNdir
{
private:
    float _voltage;
    int _voltage_dif;
    float _concentration;  
public:
    iNdir();
    ~iNdir();
    void read_value(int sensor_value);
    float get_co2_value();
};
