#!/usr/bin/env python
#coding='utf-8'

# import sys
# import csv

# input_file = "table.csv"
# output_file = "out.txt"

# with open(input_file,'r',newline='') as file_read:
#     with open(output_file,'w',newline='') as file_write:
#         file_reader = csv.reader(file_read)
#         file_writer = csv.writer(file_write)
#         header = next(file_reader)
#         row_count = 0
#         for row in file_reader:
#             print("shift_map["+str(row_count)+"] = {", end="")
#             row_count += 1
#             is_first = True
#             for i in range(len(row)):
#                 if i == 0: continue
#                 if row[i] is not '':
#                     # shift
#                     if "shift" in row[i]:
#                         if not is_first: print(",",end="")
#
#                         b = row[i].split('(')
#                         c = b[1].split(')')
#                         print("{\""+header[i]+"\","+c[0]+"}", end="")
#                     elif "reduce" in row[i]:
#                         pass
#                     else:
#                         if not is_first: print(",",end="")
#
#                         print("{\""+header[i]+"\","+row[i]+"}", end="")
#             print("};")


import sys
import csv

input_file = "table.csv"
output_file = "shift_map.txt"

with open(input_file,'r',newline='') as file_read:
    with open(output_file,'w',newline='') as file_write:
        file_reader = csv.reader(file_read)
        file_writer = csv.writer(file_write)
        header = next(file_reader)
        row_count = 0
        for row in file_reader:
            
            for i in range(len(row)):
                if i == 0: continue
                if row[i] is not '':
                    # shift
                    if "shift" in row[i]:
                        b = row[i].split('(')
                        c = b[1].split(')')
                        print("shift_map["+str(row_count)+"][\""+header[i]+"\"] = "+c[0]+";")
                    elif "reduce" in row[i]:
                        pass
                    else:
                        print("shift_map["+str(row_count)+"][\""+header[i]+"\"] = "+c[0]+";")
            row_count += 1



