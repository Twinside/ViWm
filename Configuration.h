#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

namespace ViWm
{
    class Configuration
    {
    public:
        Configuration( const char *filename );
        ~Configuration();
        
        bool    getEqualAlways() const { return equalAlways; }
        int     idleTransparency() const { return alpha; }

    private:
        bool    equalAlways;
        int     alpha;
    };
}

#endif /* __CONFIGURATION_H__ */
