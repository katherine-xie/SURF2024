Note: All macros should be run through CERN's [ROOT framework](https://root.cern/). Click the link for installation details.

## Event/Jet Display Macros (Stored in Display_Macros)
These files offer a qualatative way to view the scattering of hadrons after a collision. Three frames of reference can be displayed: laboratory frame, jet frame (with respect to the jet axis), and the boosted jet frame.
Tracks are drawn through ROOT's THelix class

### Instructions

1. Make sure datasets are stored in the .root format. There should be a branch for pT, eta, phi, charge, and PID for all particles. For jets, this should be stored in daughter branches. You can also rename the TTreeReaderValue references to match the name of your specific branch (if necessary).
2. Make sure you have downloaded the SURF_FUNCTIONS directory and updated its path.
3. Change the global variables (title and path directory).
4. Pick an event and jet index (if applicable). The code will display one event or jet at a time.
5. Run the macro.

## Two-Particle Correlation Macros (Stored in TwoParticleCorrelation_Macros)
These files create a .root file that store the corrected signal distribution (and other histograms) in angular two-particle correlation analysis.
The "simpleTwoParticleCorr.C" file performs the analysis for all two-particle pair combinations (with eta and multiplicity cut-offs). For the regular "twoParticleCorr.C" file, you can specify the parameters for a trigger and associate particle.

### Instructiomns

1. Make sure datasets are stored in the .root format. Make sure branch names match up with the TTreeReaderValue references. For the two-particle correlation in the jet frame, the data should be stored in daughter branches.
2. Make sure SURF_FUNCTIONS is installed in the correct directory.
3. Change the global variables such as the title, path directory, eta cutoff value, multiplcity cutoff value.
4. Note: You may need to calculate the number of selected pseudoparticles particles for the background distribution manually due to datatype limits.
5. Update the output file name and path.
6. Run the macro. 
