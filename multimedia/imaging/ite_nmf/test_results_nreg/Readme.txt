To generate test report from ite_nreg_datapath execution (compile .out files under ite_nmf_8500/test_results_nreg and generate html file)

Under "imaging/ite/test_report_tools"
perl create_report_ite.pl -gentestlist -path ../../ite_nmf_8500/test_results_nreg -tag <your_tag>

=> test_report_<your_tag>.html generated under "imaging/ite/test_report_tools"