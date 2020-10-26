"Syntax file for libcsv

"Functions:
syntax keyword cFunction csv_read_table
syntax keyword cFunction csv_write_table
syntax keyword cFunction csv_validate_file
syntax keyword cFunction csv_create_table
syntax keyword cFunction csv_drop_table
syntax keyword cFunction csv_alter_table_add
syntax keyword cFunction csv_alter_table_drop
syntax keyword cFunction csv_insert_record
syntax keyword cFunction csv_insert_new_record
syntax keyword cFunction csv_delete_current_record
syntax keyword cFunction csv_next_record
syntax keyword cFunction csv_rewind
syntax keyword cFunction csv_get_number_field_by_name
syntax keyword cFunction csv_get_number_field_by_index
syntax keyword cFunction csv_get_string_field_by_name
syntax keyword cFunction csv_get_string_field_by_index
syntax keyword cFunction csv_set_number_field_by_name
syntax keyword cFunction csv_set_number_field_by_index
syntax keyword cFunction csv_set_string_field_by_name
syntax keyword cFunction csv_set_string_field_by_index
syntax keyword cFunction csv_gnfbn
syntax keyword cFunction csv_gnfbi
syntax keyword cFunction csv_gsfbn
syntax keyword cFunction csv_gsfbi
syntax keyword cFunction csv_snfbn
syntax keyword cFunction csv_snfbi
syntax keyword cFunction csv_ssfbn
syntax keyword cFunction csv_ssfbi
syntax keyword cFunction is_eof
syntax keyword cFunction read_line

"Tape_Symbols:
syntax keyword cConstant STRING
syntax keyword cConstant NUMBER
syntax keyword cConstant BLANK
syntax keyword cConstant START1
syntax keyword cConstant START2
syntax keyword cConstant STOP
syntax keyword cConstant STRING_STOP
syntax keyword cConstant NUMBER_STOP

"Turing_Machine_States:
syntax keyword cConstant MASTER
syntax keyword cConstant FINAL
syntax keyword cConstant TRAP
syntax keyword cConstant END_HDR
syntax keyword cConstant N_MINUS
syntax keyword cConstant N_BEFORE
syntax keyword cConstant N_POINT
syntax keyword cConstant N_AFTER
syntax keyword cConstant S_TEXT
syntax keyword cConstant S_FINAL

"Operations:
syntax keyword cFunction move_head_left
syntax keyword cFunction move_head_right
syntax keyword cFunction head_read
syntax keyword cFunction head_write

"Transducer_States:
syntax keyword cConstant START
syntax keyword cConstant IN
syntax keyword cConstant OUT

"Field_Types:
syntax keyword cType csv_string
syntax keyword cType csv_number

"Struct_Types:
syntax keyword cType tm_tape
syntax keyword cType tm_head
syntax keyword cType csv_field
syntax keyword cType csv_record
syntax keyword cType csv_table

"Other_Macros:
syntax keyword cConstant _EOL_


"Syntax file for libdfloat

syntax keyword cType dfloat16_t
syntax keyword cType dfloat32_t
syntax keyword cType dfloat64_t
syntax keyword cType dfloat128_t

syntax keyword cFunction dfloat16_add
syntax keyword cFunction dfloat16_sub
syntax keyword cFunction dfloat16_mul
syntax keyword cFunction dfloat16_div
syntax keyword cFunction dfloat16_cmp
syntax keyword cFunction dfloat16_cpy
syntax keyword cFunction dfloat16_atof
syntax keyword cFunction dfloat16_ftoa

syntax keyword cFunction dfloat32_add
syntax keyword cFunction dfloat32_sub
syntax keyword cFunction dfloat32_mul
syntax keyword cFunction dfloat32_div
syntax keyword cFunction dfloat32_cmp
syntax keyword cFunction dfloat32_cpy
syntax keyword cFunction dfloat32_atof
syntax keyword cFunction dfloat32_ftoa

syntax keyword cFunction dfloat64_add
syntax keyword cFunction dfloat64_sub
syntax keyword cFunction dfloat64_mul
syntax keyword cFunction dfloat64_div
syntax keyword cFunction dfloat64_cmp
syntax keyword cFunction dfloat64_cpy
syntax keyword cFunction dfloat64_atof
syntax keyword cFunction dfloat64_ftoa

syntax keyword cFunction dfloat128_add
syntax keyword cFunction dfloat128_sub
syntax keyword cFunction dfloat128_mul
syntax keyword cFunction dfloat128_div
syntax keyword cFunction dfloat128_cmp
syntax keyword cFunction dfloat128_cpy
syntax keyword cFunction dfloat128_atof
syntax keyword cFunction dfloat128_ftoa

syntax keyword cFunction dfloat16_cast32
syntax keyword cFunction dfloat16_cast64
syntax keyword cFunction dfloat16_cast128
syntax keyword cFunction dfloat32_cast16
syntax keyword cFunction dfloat32_cast64
syntax keyword cFunction dfloat32_cast128
syntax keyword cFunction dfloat64_cast16
syntax keyword cFunction dfloat64_cast32
syntax keyword cFunction dfloat64_cast128
syntax keyword cFunction dfloat128_cast16
syntax keyword cFunction dfloat128_cast32
syntax keyword cFunction dfloat128_cast64

"Dfloat_free_Functions:
syntax keyword cFunction dfloat16_addf
syntax keyword cFunction dfloat16_subf
syntax keyword cFunction dfloat16_mulf
syntax keyword cFunction dfloat16_divf
syntax keyword cFunction dfloat16_cmpf
syntax keyword cFunction dfloat16_cast32f
syntax keyword cFunction dfloat16_cast64f
syntax keyword cFunction dfloat16_cast128f
syntax keyword cFunction dfloat16_ftoaf
syntax keyword cFunction dfloat32_addf
syntax keyword cFunction dfloat32_subf
syntax keyword cFunction dfloat32_mulf
syntax keyword cFunction dfloat32_divf
syntax keyword cFunction dfloat32_cmpf
syntax keyword cFunction dfloat32_cast16f
syntax keyword cFunction dfloat32_cast64f
syntax keyword cFunction dfloat32_cast128f
syntax keyword cFunction dfloat32_ftoaf
syntax keyword cFunction dfloat64_addf
syntax keyword cFunction dfloat64_subf
syntax keyword cFunction dfloat64_mulf
syntax keyword cFunction dfloat64_divf
syntax keyword cFunction dfloat64_cmpf
syntax keyword cFunction dfloat64_cast16f
syntax keyword cFunction dfloat64_cast32f
syntax keyword cFunction dfloat64_cast128f
syntax keyword cFunction dfloat64_ftoaf
syntax keyword cFunction dfloat128_addf
syntax keyword cFunction dfloat128_subf
syntax keyword cFunction dfloat128_mulf
syntax keyword cFunction dfloat128_divf
syntax keyword cFunction dfloat128_cmpf
syntax keyword cFunction dfloat128_cast16f
syntax keyword cFunction dfloat128_cast32f
syntax keyword cFunction dfloat128_cast64f
syntax keyword cFunction dfloat128_ftoaf
