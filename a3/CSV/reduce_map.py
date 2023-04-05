#!/usr/bin/env python
#coding='utf-8'

import sys
import csv

input_file = "table.csv"
output_file = "reduce_map.txt"

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
                    if "reduce" in row[i]:
                        b = row[i].split('(')
                        c = b[1].split(')')
                        d = c[0].split('>')
                        e = d[1].split(' ')
                        print("reduce_map["+str(row_count)+"][\""+header[i]+"\"] = {\""+str(len(e))+"\", \""+d[0]+"\", \""+c[0]+"\"};")
                        # print("strcpy(reduce_map["+str(row_count)+"][\""+header[i]+"\"][0], \""+str(len(e))+"\");")
                        # print("strcpy(reduce_map["+str(row_count)+"][\""+header[i]+"\"][1], \""+d[0]+"\");")
                        # print("strcpy(reduce_map["+str(row_count)+"][\""+header[i]+"\"][2], \""+c[0]+"\");")
            row_count += 1



