
Test report generation is based on mmte test Api output.
It implies constraint on scenarios (see point 1/).
For parser usage, see point 2/.

1/ Constraints on scenario
===========================
A scenario MUST have one name, with NO SPACE (declared through : mmte_testStart name_A)
It is composed of one or several assertion (declared through : mmte_testNext assert ijk) 
Assertion might have a comment, detailling failure for example (declared through : mmte_testComment blabla bleble)
Each assertion MUST have a result (declared through mmte_testResult PASSED/FAILED) 
Scenario MUST end (declared through : mmte_testEnd).

Scenario name need to be in line with Test Plan and explicit for reporting.
One script or one C-test can however contains several scenario, for example one scenario per camera resolution.

This will result in several .out files, one per testName.
Which are organised into a sub-tree when testNames contains directories. When so, sub-dirs MUST exist before running test.


2/ Tools usage
===============
- FOR SYMBIAN on your Windows PC:
   * cygwin_setup_GNB.reg
   * Ite_Symbian_Report_GNB.bat 
      use it to launch create_report_ite.pl   
      for example: Ite_Symbian_Report_GNB.bat -t lst_omx -o Test --html Test -p NDK20 -w W932 
      
- dispose of several .out from mmte runs, from a given campaign, organised in a sub-tree.
- copy them, keep sub-tree structure, under an identified directory (TBC).
- launch tool to get html report(s). 
	When several reports, depending on .out sub-tree structure, they are then also concatened.

perl create_report_ite.pl [-tag campaign_id] -path <relative/path/to/outfiles>
			[-gentestlist | -testlist <list_dir>] [-primarycam|secondarycam] 
			[-help] [-verbose] 

Options precising
------------------
path is mandatory to specify where to find the .out files and sub-tree to parse
tag is mandatory, useful to identified tested version (default ALPHA)

gentestlist is optional, allows to generate reports depending on sub-tree, from all the tests under path
testlist is optional, allows to produce one report for each testlists under path
	default is generate testlists

When gentestlist, that is mostly nmf tests, you can select which camera results to report.
Use -primaraycam or -secondarycam option for that. 
None will result in both camera in a single report, separated in different groups.

Example OMX tests :
	perl create_report_ite.pl -testlist lst_omx -path ~/8500_resultat/out_dir -tag XX [-v]

Example NMF tests :
	perl create_report_ite.pl -gentestlist  -path ~/8500_resultat/out_dir -tag XX [-primarycam|secondarycam] [-v]



3/ Behaviour
===============
We distinguish 2 kinds of results: one for .out directly under result folder, the other for .out under one sub-tree.

3-1 first level files
MMTE resulting files .out are parsed, using parse_out_ite.pl script. This allows to generate .rpt files, one per .out.
After what, the .rpt files are concatenate to one Basic database and one html report.

3-2 sub-tree structured result
Results are grouped by first level directories, so that one dir directly under result corresponds to one database and one high level report.
Then the .out files directly in the first level directory are considered as one test by file, parsed one by one.
On the contrary, the .out files in a same sub-sub-directory are considered as one single test entry, each file corresponding to one assertion. Doing so we reduce the number of entries visible to html report to one per significant result folder.


4/ Files storing
=================
No storage are yet identified.

.out files
-------------
At OMX level, 
they are created under ite_omx_use_cases/test_report folder. Run tool on one concerned sub-folder as path.
At NMF level, 
they are created under ite_nmf_8500/test_results_nreg folder.

Html reports
-------------
At OMX level, 
generate report are available under Codex, inside imaging_osi/<week>/ product folder.
At NMF level,
generated reports are not saved, no backup, nor copy, nor even published. 
They are only in your local directory. Up to you to publish them.


