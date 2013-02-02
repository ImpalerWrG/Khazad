#ifndef SETTLMENT_HEADER
#define SETTLMENT_HEADER

#include <stdafx.h>

#include <DataManager.h>
#include <Citizen.h>
#include <Coordinates.h>
#include <OgreVector3.h>
#include <MovementController.h>

class Settlment
{
    public:
        Settlment();
        virtual ~Settlment();
        void InitializeSettlment();

    protected:

        std::vector<Citizen*> SettlmentMember;

        void addMember(Citizen* NewMember);
        void removeMember(Citizen* DepartingMember);

    private:

};

#endif // SETTLMENT_HEADER
