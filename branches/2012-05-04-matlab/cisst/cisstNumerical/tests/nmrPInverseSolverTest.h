/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
  Author(s):  Ankur Kappor
  Created on: 2005-07-28
  
  (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _nmrPInverseSolverTest_h
#define _nmrPInverseSolverTest_h

// The class nmrSVDSolver is obsolete but we still want to test it for a while
#include <cisstCommon/cmnPortability.h>
#ifdef CISST_COMPILER_IS_MSVC
#pragma warning (disable: 4996)
#endif

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cisstNumerical/nmrPInverseSolver.h>

class nmrPInverseSolverTest  : public CppUnit::TestCase
{
    CPPUNIT_TEST_SUITE(nmrPInverseSolverTest);
    CPPUNIT_TEST(TestSolveBookExample);
    CPPUNIT_TEST_SUITE_END();
protected:
    vctDynamicMatrix<double> Input;  // A
    vctDynamicMatrix<double> Output; // A^+

public:

    void setUp() {
        Input.SetSize(15, 15, VCT_COL_MAJOR);
	Input.Assign(-71.64286237,-72.14904772,47.32856122,-49.77298052,8.090071121,22.92883361,-100.2605554,62.49751762,-40.59770033,-10.72001048,-95.94970264,-38.35161572,-81.50021577,-132.3502962,75.32246175,
			-65.55593895,-20.11809995,11.69456568,-111.8716638,-108.105649,-26.66231361,-18.56206729,-104.7337938,-153.4894549,-97.7056672,-14.60425882,-52.84798039,36.66142697,-66.15766766,6.498934148,
			31.43627633,-2.04641611,-59.11038386,80.76496194,-112.4517812,70.16721769,-105.4032714,153.5670381,22.13731024,-96.39881166,74.45380272,5.538836427,-58.61067585,-14.6114505,-29.27642587,
			10.68140759,27.88899999,-65.47076751,4.119957864,173.5676343,-48.75904913,-7.153948892,43.44257379,-137.4478654,-237.9172353,-89.04960333,125.3768571,153.7409026,24.80849582,8.28228472,
			184.8216218,105.8294814,-108.0661851,-75.62086055,193.7458597,186.2479725,27.91984197,-191.7135853,-83.92855308,-83.81879147,13.90618587,-252.0003639,14.00715285,-7.663255738,76.61914803,
			-27.51056754,62.16732824,-4.77308653,-8.912914781,163.5068219,110.685111,137.3275342,46.99395244,-20.86425597,25.7345635,-23.61442972,58.48561204,-186.2766666,173.816997,223.6849878,
			221.2554079,-175.0615288,37.93445373,-200.8850322,-125.5940167,-122.7565722,17.98410322,127.4351133,75.59129988,-18.38340001,-7.545912903,-100.8064417,-45.4193097,162.19719,32.68867252,
			150.8525756,69.73475515,-33.03610458,108.3918038,-21.35375062,-66.98851101,-54.20165565,63.85424511,37.57344526,-16.76152962,-35.85719128,94.42848246,-65.20741052,62.64358968,86.33039427,
			-194.50786,81.14858633,-49.98982512,-98.11905631,-19.89320483,134.0929452,163.4190581,138.0782476,-134.5412966,-11.69889087,-207.7634855,-242.3957134,10.33178769,9.181359023,67.93866017,
			-168.0542778,63.63449476,-3.597860795,-68.84886375,30.74991775,38.80833162,82.52151515,131.9842856,148.1876246,16.84881738,-14.35457102,-22.38314285,-22.0631622,-80.76070465,55.47576259,
			-57.35341341,131.0080341,-17.47603312,133.9479482,-57.23254565,39.30589288,23.07611423,-90.94292365,3.273613758,-50.12062963,139.3341475,5.806988274,-27.90433666,-46.13370942,100.1630427,
			-18.58165274,32.70975159,-95.72650788,-90.92431603,-97.7648367,-170.7333578,67.16339459,-230.5605052,187.0452825,-70.50764235,65.18040917,-42.46140151,-73.3662013,-140.5969194,125.9365148,
			0.8934115677,-67.29931639,129.25479,-41.28577286,-44.68094067,22.78586447,-50.80778804,178.8730245,-120.8990524,50.8164684,-37.71335577,-20.29179453,-6.453358017,-37.45296327,4.415106519,
			83.69498908,-14.93274995,44.09096428,-50.61631856,108.2091901,68.56328578,85.63515973,39.07978083,-78.26318678,-42.09215358,-66.14430595,-151.3076979,-144.4004066,-47.09112363,-31.41376747,
			-72.22706724,-244.9017754,128.0940943,161.9747799,237.2647949,-63.67901129,26.85034535,2.032367637,-76.72985241,22.91333721,24.89579762,-112.6351861,61.23402922,175.1295626,22.67076409);
	Output.SetSize(15, 15, VCT_COL_MAJOR);
	Output.Assign(0.003429616902,-0.008786757876,-0.002545810181,0.002685826319,-0.000476391875,-0.00102561559,0.003349246852,-0.004524152035,0.001186286154,-0.00609155689,0.005388743445,0.0003399496701,0.0008253253856,0.0006389542233,-0.002637430624,
			-0.008298302009,0.01549584745,0.001093805328,-0.004656989186,0.002965628019,-0.0003175779593,-0.004667279332,0.01323768672,-0.003309873132,0.01242237949,-0.007226023553,-0.002537114274,0.0002062539478,0.001072632607,0.002660221314,
			0.008645066272,0.001000647708,-0.009168672841,0.0005380155169,-0.002392137387,-0.003641972795,0.007421446617,-0.0001620547637,-0.002026237182,0.00345155025,0.01275755237,-0.01028296396,-0.01008027644,0.003740488146,-0.001582013194,
			0.00470257003,-0.008206619746,-0.001841708762,0.001973192772,-0.002182315627,-0.001758911903,0.002132024862,-0.00347093681,0.002487309296,-0.005908515542,0.007021105266,-0.0005558741318,-0.002384435624,0.0008391212955,-0.0007984029438,
			-0.005615869262,0.006113280133,0.002763198196,-0.001497404893,0.002669062241,0.001390130016,-0.004757775813,0.005156072084,-0.002478651456,0.005774957073,-0.008117984603,0.002034763033,0.003649515985,0.00019332399,0.002927742548,
			0.01278201178,-0.01406964479,-0.005459643456,0.004147989263,-0.002644547415,-0.001178080825,0.00809913896,-0.01273126932,0.002497131456,-0.009085663228,0.01440063461,-0.005674512083,-0.007943145829,6.124026247e-005,-0.005357047534,
			0.003947899029,-0.01507801264,-0.003656226315,0.005135663257,-0.003687288182,-0.0001018809928,0.004451994798,-0.01160766438,0.004034867753,-0.01047442219,0.009541024268,0.002374411944,0.0009219700316,0.001723351403,-0.003738027013,
			-0.005253543407,0.001462494268,0.004059010813,-0.0002747882669,0.001093503122,0.0009066536675,-0.0028322215,0.002549375784,5.192275677e-006,0.002599920138,-0.005232057921,0.003127533584,0.006472522453,-0.0006105858335,0.001370240547,
			0.007894551028,-0.004753140532,-0.004752874716,0.00113332053,-0.001647097333,-0.002691939027,0.005926147955,-0.002838508578,-4.603143866e-005,0.0005812824349,0.008232432947,-0.005363788221,-0.009291744024,0.0008633262602,-0.001882039817,
			-0.006593092848,0.0004588590625,0.002977740683,-0.002810666239,0.002064654337,0.002074237187,-0.005599404224,0.002136621368,0.0003804328555,-0.0005527495848,-0.008746135551,0.005558600282,0.009044989244,-0.002906334672,0.001327241412,
			-0.01039109512,0.006857848022,0.006587093882,-0.002357129665,0.003014908364,0.002642402985,-0.005545347766,0.003103469917,-0.002761323821,0.004732618206,-0.008402460819,0.005194124072,0.009279254833,-0.001410065108,0.003045361498,
			0.004239265254,-0.007474858361,-0.002234009588,0.003332607527,-0.00253823298,0.001122520129,0.002279232563,-0.006610682241,0.0004479414623,-0.005914688355,0.004636350486,-0.000220945734,-0.0008020493555,-0.0004517198609,-0.003250012205,
			0.004460083499,-0.008714834924,-0.0040186851,0.0036137479,-0.0003854096236,-0.002317131844,0.004081239392,-0.006069511672,0.002119966775,-0.004355351706,0.007232965694,-0.001186281602,-0.0003193304123,-0.002590332191,-0.002026084991,
			-0.002798482955,0.009823543468,0.0004997045059,-0.00310049079,0.0007702117564,0.0007289714059,2.6715425e-006,0.006511136342,-0.001678099298,0.006201367119,-0.003181063402,-0.004337686012,-0.005083365623,-0.0003171865593,0.002220720004,
			0.004738347406,-0.006958582405,-0.002053758738,0.002389060456,4.210807788e-005,0.0007234225667,0.002696532848,-0.003824190812,0.001832328917,-0.004486467501,0.005442918811,0.001560170621,0.002037206761,-0.002734152376,-0.001181410817);
    }

    void tearDown()
    {}

    /*! Test based on the example provided with the Lawson and Hanson book */
    void TestSolveBookExample(void);
};


#endif // _nmrPInverseSolverTest_h

