#ifndef MANUALVIMLAYOUT_H
#define MANUALVIMLAYOUT_H

#include "../Layouter.h"
#include "../Configuration.h"

namespace ViWm {
namespace Layout
{
    class ManualVimLayout : public Layouter
    {
    public:
        ManualVimLayout( const Configuration &c );

        virtual void layout( const WindowMakerState &s, DesktopLayout &l );
        virtual void reLayout( const Screen &s, LayoutNode &n );
        virtual void addNewWindowToLayout( TilledWindow &newWindow
                                         , const WindowMakerState &st
                                         , DesktopLayout          &l
                                         );
    private:
        void operator =(const ManualVimLayout &);

        const   Configuration &conf;
        void    normalizeNode( const Screen &s, LayoutLeaf *leaf, LayoutNode* node );
    };
}}

#endif /* MANUALVIMLAYOUT_H */
