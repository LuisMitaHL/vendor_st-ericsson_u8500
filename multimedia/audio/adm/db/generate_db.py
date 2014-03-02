#! /usr/bin/env python

"""Create and populate the adm data base.

   INIT_FILE        - Defines the tables.
   HW_HANDLERS_FILE - Contains the hw settings data. NOTE that this file have references to
                      the *.state files.
   IL_CFGDEF_FILE   - This file is created the ADM is built. It contains data describing the
                      structs of the settings types.
   ADM_DATA_FILE    - Containes the remainder of the data.

   The variable 'test_data' can be defined to include data for testing, see below.
"""

import sys, re, os, sqlite3


# $(LOCAL_PATH) $(ADM_PATH)
BUILD_PATH = sys.argv[1] + "/";
ADM_PATH   = sys.argv[2] + "/";
DB_VARIANT = sys.argv[3]

DB_FILE = BUILD_PATH + 'adm.sqlite-tmp-' + DB_VARIANT
INIT_FILE = ADM_PATH + 'initiate_db.txt'
ADM_DATA_FILE = ADM_PATH + 'main_data.txt'
HW_HANDLERS_FILE = ADM_PATH + 'hw_handlers.txt'
IL_CFGDEF_FILE = BUILD_PATH + 'il_cfgdef.txt'

print "  ADM database, build path:  " + BUILD_PATH;
print "  ADM database, source path: " + ADM_PATH;


# Include test data by pointing out a file containing test data
# test_data = 'db/test_data.txt'
test_data = None


def strip_line(line):
    line_s = line.lstrip().rstrip()
    return line_s

def valid_line(line):
    """Lines starting with ';' or '!' are not valid
    """
    if not line:
        return False
    line_s = strip_line(line)
    if not line_s or line_s[0] == ';' or line_s[0] == '!':
        return False

    return True

def create():
    """Creates the database and it's tables.
    """
    if os.path.exists(DB_FILE):
        os.remove(DB_FILE)
    con = sqlite3.connect(DB_FILE)
    c = con.cursor()
    c.execute('''PRAGMA foreign_keys = ON''')

    initiate_db = open(INIT_FILE, 'r')
    conc_line = ''
    for line in initiate_db:
        if not valid_line(line):
            continue

        # Incomplete line must be concatenated, i.e. logical lines ends with ';'
        if len(conc_line) == 0:
            # First one - remove leading white spaces
            conc_line = line.lstrip()
        else:
            conc_line += '\n' + line

        if conc_line.rstrip()[-1] == ';':
            c.execute(conc_line.rstrip())
            conc_line = ''

    initiate_db.close()
    con.commit()
    c.close()

def populate():
    """ Inserts all the data into the database
    """

    if not os.path.exists(DB_FILE):
        print "ERROR. " + sys.argv[0] + " Database was not created. Can't populate. Exit."
        exit(20)

    con = sqlite3.connect(DB_FILE)
    c = con.cursor()
    c.execute('''PRAGMA foreign_keys = ON''')

    # Insert HW_Settings data
    sp = re.compile('''<(.+)>''') # Regexp used to find the state file
    hw_handler = open(HW_HANDLERS_FILE, 'r')
    for line in hw_handler:
        if not valid_line(line):
            continue

        line_s = strip_line(line)

        # Replace <x.state> with the content of the file x.state
        if '<' in line_s:
            state_file_name = sp.search(line_s).groups()[0]
            state_data = open(ADM_PATH + state_file_name, 'rb').read()
            state_data = state_data.replace('\n', ' ')
            state_data = state_data.replace('\t', ' ')
            state_data = state_data[0:-1]
            line_s = line_s.replace('<' + state_file_name + '>', '"' + state_data + '"')
        try:
            c.execute(line_s)
        except Exception, e:
            print 'Error: %s' % e
            print 'when executing: ' + line_s
            exit(20)

    hw_handler.close()

    # Insert basic data
    main_data = open(ADM_DATA_FILE, 'r')
    effect_chain_table_name = 'EffectChainDef'
    effect_chain = []
    for line in main_data:
        if not valid_line(line):
            continue

        line_s = strip_line(line)

        # Insert EffectChainDef last since they have a referce to Template_OMXName
        if effect_chain_table_name in line_s:
            effect_chain.append(line_s)
        else:
            try:
                c.execute(line_s)
            except Exception, e:
                print 'Error: %s' % e
                print 'when executing: ' + line_s
                exit(20)
    main_data.close()

    for line in effect_chain:
        try:
            c.execute(line)
        except Exception, e:
            print 'Error: %s' % e
            print 'when executing: ' + line
            exit(20)

    # Insert StructDef data
    try:
        struct_def = open(IL_CFGDEF_FILE, 'r')
    except:
        print 'ERROR. ' + sys.argv[0] + ' could not open file: ' + IL_CFGDEF_FILE
        print 'Aborting.'
        exit(20)

    for line in struct_def:
        if valid_line(line):
            try:
                c.execute(strip_line(line))
            except Exception, e:
                print 'Error: %s' % e
                print 'when executing: ' + line
                exit(20)

    struct_def.close()

    # Test data
    if test_data:
        test_data_file = open(test_data, 'r')
        for line in test_data_file:
            if valid_line(line):
                try:
                    c.execute(strip_line(line))
                except Exception, e:
                    print 'Error: %s' % e
                    print 'when executing: ' + line
                    exit(20)

        test_data_file.close()

    # Done
    con.commit()
    c.close()

def main():
    print DB_VARIANT + ': Creating database...'
    create()
    print DB_VARIANT + ': Done.'
    print DB_VARIANT + ': Populating database...'
    populate()
    print DB_VARIANT + ': Done.'


if __name__ == '__main__':
    main()

