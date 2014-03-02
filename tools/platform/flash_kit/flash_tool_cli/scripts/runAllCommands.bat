java -jar flashtool.jar -port 8088 -host localhost reload_configuration 
sleep 0.2
java -jar flashtool.jar -port 8088 -host localhost reload_configuration 
sleep 0.2
java -jar flashtool.jar -port 8088 -host localhost enable_auto_sense
sleep 0.2
java -jar flashtool.jar -port 8088 -host localhost disable_auto_sense
sleep 0.2
java -jar flashtool.jar -port 8088 -host localhost get_auto_sense_status
sleep 0.2
java -jar flashtool.jar -port 8088 -host localhost enable_auto_initialize
sleep 0.2
java -jar flashtool.jar -port 8088 -host localhost disable_auto_initialize
sleep 0.2
java -jar flashtool.jar -port 8088 -host localhost get_auto_initialize_status
sleep 0.2
java -jar flashtool.jar -port 8088 -host localhost set_active_profile -profile_name value_of_profile_name
sleep 0.2
java -jar flashtool.jar -port 8088 -host localhost get_active_profile
sleep 0.2
java -jar flashtool.jar -port 8088 -host localhost get_available_profiles
sleep 0.2
java -jar flashtool.jar -port 8088 -host localhost get_connected_equipments
sleep 0.2
java -jar flashtool.jar -port 8088 -host localhost get_next_connected_equipment
sleep 0.2
java -jar flashtool.jar -port 8088 -host localhost get_core_dump_location
sleep 0.2
java -jar flashtool.jar -port 8088 -host localhost set_core_dump_location -path value_of_path
sleep 0.2
java -jar flashtool.jar -port 8088 -host localhost initialize_equipment -equipment_id value_of_equipment_id
sleep 0.2
java -jar flashtool.jar -port 8088 -host localhost shutdown_equipment -equipment_id value_of_equipment_id
sleep 0.2
java -jar flashtool.jar -port 8088 -host localhost reboot_equipment -equipment_id value_of_equipment_id -mode value_of_mode
sleep 0.2
java -jar flashtool.jar -port 8088 -host localhost execute_software -equipment_id value_of_equipment_id -path value_of_path
sleep 0.2
java -jar flashtool.jar -port 8088 -host localhost authenticate_certificate -equipment_id value_of_equipment_id -sign_package_name value_of_sign_package_name
sleep 0.2
java -jar flashtool.jar -port 8088 -host localhost authenticate_control_keys -equipment_id value_of_equipment_id -key_set value_of_key_set
sleep 0.2
java -jar flashtool.jar -port 8088 -host localhost process_file -equipment_id value_of_equipment_id -path value_of_path
sleep 0.2
java -jar flashtool.jar -port 8088 -host localhost dump_area -equipment_id value_of_equipment_id -area_path value_of_area_path -offset value_of_offset -length value_of_length -path value_of_path
sleep 0.2
java -jar flashtool.jar -port 8088 -host localhost erase_area -equipment_id value_of_equipment_id -area_path value_of_area_path -offset value_of_offset -length value_of_length
sleep 0.2
java -jar flashtool.jar -port 8088 -host localhost write_global_data_set -equipment_id value_of_equipment_id -storage_id value_of_storage_id -path value_of_path
sleep 0.2
java -jar flashtool.jar -port 8088 -host localhost read_global_data_set -equipment_id value_of_equipment_id -storage_id value_of_storage_id -path value_of_path
sleep 0.2
java -jar flashtool.jar -port 8088 -host localhost erase_global_data_set -equipment_id value_of_equipment_id -storage_id value_of_storage_id
sleep 0.2
java -jar flashtool.jar -port 8088 -host localhost write_global_data_unit -equipment_id value_of_equipment_id -storage_id value_of_storage_id -unit_id value_of_unit_id -unit_data value_of_unit_data
sleep 0.2
java -jar flashtool.jar -port 8088 -host localhost read_global_data_unit -equipment_id value_of_equipment_id -storage_id value_of_storage_id -unit_id value_of_unit_id
sleep 0.2
java -jar flashtool.jar -port 8088 -host localhost set_domain -equipment_id value_of_equipment_id -domain value_of_domain
sleep 0.2
java -jar flashtool.jar -port 8088 -host localhost get_domain -equipment_id value_of_equipment_id
sleep 0.2
java -jar flashtool.jar -port 8088 -host localhost get_equipment_properties -equipment_id value_of_equipment_id
sleep 0.2
java -jar flashtool.jar -port 8088 -host localhost set_equipment_properties -equipment_id value_of_equipment_id -equipment_properties value_of_equipment_properties
sleep 0.2
java -jar flashtool.jar -port 8088 -host localhost bind_properties -equipment_id value_of_equipment_id
sleep 0.2
java -jar flashtool.jar -port 8088 -host localhost read_bits -equipment_id value_of_equipment_id
sleep 0.2
java -jar flashtool.jar -port 8088 -host localhost set_bits -equipment_id value_of_equipment_id -bit_array value_of_bit_array

pause
