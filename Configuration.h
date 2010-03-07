#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

namespace ViWm
{
    class Configuration
    {
    public:
        Configuration( const char *filename );
        ~Configuration();
        
        bool    getEqualAlways() const { return equalAlways; };

    private:
        bool    equalAlways;
    };
}

#endif /* __CONFIGURATION_H__ */
