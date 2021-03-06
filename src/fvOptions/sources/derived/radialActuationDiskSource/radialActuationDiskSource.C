/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011-2013 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "radialActuationDiskSource.H"
#include "geometricOneField.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace fv
{
    defineTypeNameAndDebug(radialActuationDiskSource, 0);
    addToRunTimeSelectionTable
    (
        option,
        radialActuationDiskSource,
        dictionary
    );
}
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::fv::radialActuationDiskSource::radialActuationDiskSource
(
    const word& name,
    const word& modelType,
    const dictionary& dict,
    const fvMesh& mesh
)
:
    actuationDiskSource(name, modelType, dict, mesh),
    radialCoeffs_(coeffs_.lookup("coeffs"))
{
    Info<< "    - creating radial actuation disk zone: " << name_ << endl;
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::fv::radialActuationDiskSource::addSup
(
    fvMatrix<vector>& eqn,
    const label fieldI
)
{
    bool compressible = false;
    if (eqn.dimensions() == dimForce)
    {
        compressible = true;
    }

    const scalarField& cellsV = mesh_.V();
    vectorField& Usource = eqn.source();
    const vectorField& U = eqn.psi();

    if (V_ > VSMALL)
    {
        if (compressible)
        {
            addRadialActuationDiskAxialInertialResistance
            (
                Usource,
                cells_,
                cellsV,
                mesh_.lookupObject<volScalarField>("rho"),
                U
            );
        }
        else
        {
            addRadialActuationDiskAxialInertialResistance
            (
                Usource,
                cells_,
                cellsV,
                geometricOneField(),
                U
            );
        }
    }
}


void Foam::fv::radialActuationDiskSource::writeData(Ostream& os) const
{
    actuationDiskSource::writeData(os);
}


bool Foam::fv::radialActuationDiskSource::read(const dictionary& dict)
{
    if (option::read(dict))
    {
        coeffs_.readIfPresent("diskDir", diskDir_);
        coeffs_.readIfPresent("Cp", Cp_);
        coeffs_.readIfPresent("Ct", Ct_);
        coeffs_.readIfPresent("diskArea", diskArea_);
        coeffs_.lookup("coeffs") >> radialCoeffs_;
        return true;
    }
    else
    {
        return false;
    }
}


// ************************************************************************* //
