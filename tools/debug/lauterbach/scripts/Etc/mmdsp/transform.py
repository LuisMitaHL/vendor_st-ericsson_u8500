import array
from collections import deque
import logging
import os, os.path
import pprint
import re
import string
import sys

log = logging.getLogger("mmdsp_coredump")

def config_logger(level = logging.INFO):
    # create logger
    log.setLevel(level)
    # create console handler and set level to debug
    ch = logging.StreamHandler()
    ch.setLevel(level)
    # create formatter
    #formatter = logging.Formatter("%(asctime)s - %(name)s - %(levelname)s - %(message)s")
    formatter = logging.Formatter("%(levelname)s: %(message)s")
    # add formatter to ch
    ch.setFormatter(formatter)
    # add ch to logger
    log.addHandler(ch)

class MemoryFile:
    """MemoryFile is the abstraction of a binary file from which we read/write sequences of bytes."""
    def __init__(self, file, mode):
        self.file = open(file, mode)
    def read(self, width):
        #return self.file.read(width)
        values = array.array('B')
        values.fromfile(self.file, width)
        return values
    def write(self, bytes):
        return bytes.tofile(self.file)
    def seek(self, offset):
        return self.file.seek(offset)
    def close(self):
        self.file.close()

class Transformation(object):
    """
    Base class for transformations.

    __call__() does the transformation by calling transform() (implemented in derived classes).
    """
    def __init__(self):
        pass
    def __call__(self, *args, **kwargs):
        self.transform(*args, **kwargs)
    def transform(self, file, workdir, *args, **kwargs):
        pass

class MemoryTransformation(Transformation):
    """
    Memory Transformation logic.

    transform() reads a coredump, 8 bytes always
    and calls swap() to perform the logic implemented in derived classes
    it writes back the result to a new file.
    """
    def __init__(self, pattern):
        self.pattern = pattern
    def __call__(self, file, *args, **kwargs):
        self.transform(file, *args, **kwargs)
    def reverse(self, bytes):
        """Reverse endianness of a 8 byte sequence."""
        tmp = array.array('B')
        tmp.append(bytes[3])
        tmp.append(bytes[2])
        tmp.append(bytes[1])
        tmp.append(bytes[0])
        tmp.append(bytes[7])
        tmp.append(bytes[6])
        tmp.append(bytes[5])
        tmp.append(bytes[4])
        return tmp
    def transform(self, file, workdir, *args, **kwargs):
        name = self.pattern.format(*args)

        log.info('Creating output for {0} -> {1}'.format(file, name))

        if workdir:
            file = os.path.join(workdir, file)
            name = os.path.join(workdir, name)

        input = MemoryFile(file, 'rb')
        output = MemoryFile(name, 'w+b')

        #read file and call swap() on every 8 bytes, write result to output
        try:
            bytes = input.read(8)
            while True:
                if kwargs.get('reverse', False):
                    bytes = self.reverse(bytes)
                tmp = self.swap(bytes)
                output.write(tmp)
                bytes = input.read(8)
        except EOFError:
            #TODO handle correctly eof
            #output.write(bytes)
            pass
        output.close()
        input.close()
    def swap(self, bytes):
        return bytes

class TransformCode(MemoryTransformation):
    """ Swap two words of 32. """
    def swap(self, bytes):
        tmp = array.array('B')
        tmp.append(bytes[4])
        tmp.append(bytes[5])
        tmp.append(bytes[6])
        tmp.append(bytes[7])
        tmp.append(bytes[0])
        tmp.append(bytes[1])
        tmp.append(bytes[2])
        tmp.append(bytes[3])
        return tmp

class TransformData24(MemoryTransformation):
    """Discard one byte of the word."""
    def swap(self, bytes):
        tmp = array.array('B')
        #tmp.append(bytes[0])
        tmp.append(bytes[1])
        tmp.append(bytes[2])
        tmp.append(bytes[3])
        #tmp.append(bytes[4])
        tmp.append(bytes[5])
        tmp.append(bytes[6])
        tmp.append(bytes[7])
        return tmp

class TransformData16(MemoryTransformation):
    """ Add padding and swap bytes in the word."""
    def swap(self, bytes):
        tmp = array.array('B')
        #add padding, swap bytes
        tmp.append(0)
        tmp.append(bytes[2])
        tmp.append(bytes[3])
        tmp.append(0)
        tmp.append(bytes[0])
        tmp.append(bytes[1])
        tmp.append(0)
        tmp.append(bytes[6])
        tmp.append(bytes[7])
        tmp.append(0)
        tmp.append(bytes[4])
        tmp.append(bytes[5])
        return tmp

class ExtractRegisterFile(Transformation):
    """Extract Register File from a coredump, situated at an offset in memory."""
    class MagicNumberError(Exception):
        def __init__(self, magic, expected):
            self.magic = magic
            self.expected = expected

    def __init__(self, pattern, regs):
        self.pattern = pattern
        self.regs = regs
    def reverse(self, bytes):
        """Reverse endianness of a 4 byte sequence."""
        tmp = array.array('B')
        tmp.append(bytes[3])
        tmp.append(bytes[2])
        tmp.append(bytes[1])
        tmp.append(bytes[0])
        return tmp
    def transform(self, file, workdir, *args, **kwargs):
        def swap_sp(name, value):
            """Infer the sp and the mode from the value."""
            if value < 0x1000: #TODO, use correct value
                real = 'sp0s'
                mode = 1
            else:
                real = 'sp0u'
                mode = 0
            return (real, mode)
        def tostr(bytes):
            """Format array of 4 bytes as a human-readable string."""
            s = [hex(byte) for byte in bytes]
            #concatenation, substition or an intermediate list and the join ...
            return "[{0}]".format(string.join(s, ', '))
        def tovalue(bytes, reverse):
            if reverse:
                bytes = self.reverse(bytes)
            value = (bytes[1] << 16) | (bytes[2] << 8) | (bytes[3])
            return value
        def check_magic(seq, magic):
            """ Check a sequence of values against a sequence of magic numbers."""
            if len(seq) != len(magic):
                raise Exception('Internal error: sequence and magic sequence not of same length')
            for i in range(len(seq)):
                if seq[i] != magic[i]:
                    return False
            return True
        def find_magic(input, seq, offset, max, reverse):
            """
            Find magic number sequence in the input stream.
            
            input - the input stream. is modified during the check
            seq   - magic number sequence, a tuple
            offset - starting offset where detection should start
            max - detection stop
            reverse - if the stream should be reverse endianness
            """
            log.info('Starting magic number detection at offset {0}'.format(hex(offset)))
            
            #seek to the specified offset or search for magic sequence
            input.seek(offset*4)
            
            #compare sequence of read bytes against the sequence of magic numbers
            #d holds a sequence of bytes read from the stream, len(d) is len(seq)
            d = deque()
            # initialize prior entering the check loop
            for i in range(0, len(seq)):
                d.append(tovalue(input.read(4), reverse))
            while offset <= max:
                if check_magic(d, seq):
                    log.info('Magic number detected at offset {0}'.format(hex(offset)))
                    return offset
                else:
                    # remove the oldest word
                    d.popleft()
                    # append to the right a new word
                    d.append(tovalue(input.read(4), reverse))
                    # loop
                    offset = offset + 1
                #log.debug(d)
            raise ExtractRegisterFile.MagicNumberError(0, 0)

        name = self.pattern.format(*args)

        log.info('Extracting registers from {0} -> {1}'.format(file, name))

        if workdir:
            file = os.path.join(workdir, file)
            name = os.path.join(workdir, name)

        reverse = kwargs.get('reverse', False)
        
        input = MemoryFile(file, 'rb')
        output = open(name, 'w+')
        
        try:
            #seek to the specified offset or search for magic sequence
            magic = (0xaaa, 0xbfa)
            offset = kwargs.get('offset', 0x0)
            offset = find_magic(input, magic, offset, 0x1000, reverse) #input is modified
            input.seek((offset + len(magic))*4) 
        except EOFError:
            log.error('EOF Error')
            input.close()
            output.close()
            return
        except ExtractRegisterFile.MagicNumberError:
            log.error('Magic number check failed')
            input.close()
            output.close()
            return

        #read register file from here
        try:
            for reg in regs:
                bytes = input.read(4)
                if reverse:
                    bytes = self.reverse(bytes)
                #drop the first byte and assemble the rest to a 24-bit value
                val = (bytes[1] << 16) | (bytes[2] << 8) | (bytes[3])
                if reg.ignored == True:
                    log.debug('ignoring register {0} = {1} ({2})'.format(reg.name, hex(val), tostr(bytes)))
                    continue

                if reg.name == 'sp0' or reg.name == 'sp_other':
                    (real, mode) = swap_sp(reg.name, val)
                    if reg.name == 'sp0':
                        log.debug('register k = {0} (auto)'.format(mode))
                        output.write('r.s k {0}\n'.format(mode))
                    reg.name = real

                log.debug('register {0} = {1} ({2})'.format(reg.name, hex(val), tostr(bytes)))
                output.write('r.s {0} {1}\n'.format(reg.name, hex(val)))
        except EOFError:
            log.error('EOF Error')
        except ExtractRegisterFile.MagicNumberError:
            log.error('Magic number check failed')
        input.close()
        output.close()

class Register():
    """Register abstraction."""
    def __init__(self, name, include = True):
        self.name = name
        self.ignored = not include

# configuration classes ########################################################################################
class Config(object):
    """ Configuration base class. One input file, one output file."""
    def __init__(self, name, trans):
        self.name = name
        self.trans = trans
    def exists(self, file, workdir):
        if workdir:
            file = os.path.join(workdir, file)
        if os.path.exists(file):
            return True
        return False
    def get(self, workdir, mpc):
        # return a list of one tuple
        if self.exists(self.name, workdir):
            return [(self.name, workdir, ),]
        else:
            return [()]

class ConfigPattern(Config):
    """Configuration accepting a pattern as input and several output files."""
    def __init__(self, pattern, trans, *args):
        super(ConfigPattern, self).__init__(pattern, trans)
        self.args = args
    def get(self, workdir, mpc):
        # return a list of tuples with performed substitution in names
        #patterns = [(self.name.format(subst), workdir, subst) for subst in mpc if self.exists(self.name.format(subst), workdir)]
        #result = [p for p in patterns if self.exists(p[0], p[1])]
        patterns = [(self.name, workdir, mpc)]
        return patterns

def traverse(config, workdir, mpc, **kwargs):
    """ Algorithm to traverse configuration list and apply the specified operations. """
    for entry in config:
        #construct a list of tuples, each tuple is of form (file, workdir, ...)
        #where ... are additionnal arguments that will be used to expand the name pattern of the file
        entries = entry.get(workdir, mpc)

        #finaly, perform the transformation on the files/arguments constructed above
        for e in entries:
            transformations = entry.trans
            if len(e) == 0:
                continue
            for trans in transformations:
                trans(*e, **kwargs)

def get_info(workdir):
    """ Return information about the mmdsp coredump found in workdir. """
    meta = os.path.join(workdir, 'metadata.txt')
    if not os.path.exists(meta):
        log.error("Meta data for coredump do not exist (metadata.txt)")
        sys.exit(1)
    dsp = None
    with open(meta) as f:
        line = f.readline()
        while line:
            m = re.match('Panic (?P<dsp>SVA|SIA)', line)
            if m:
                dsp = m.group('dsp')
                break
            line = f.readline()
    if dsp == None:
        log.error('Unknown DSP, using default SVA')
        dsp = 'SVA'
    return (dsp,)

# configuration ################################################################################################
#CAUTION
#    this is aligned with how the registers are written to memory during panic
#    in reverse order
regs = [
        Register('pc'),
        Register('max3'),
        Register('max2'),
        Register('max1'),
        Register('min3'),
        Register('min2'),
        Register('min1'),
        Register('axx3'),
        Register('axx2'),
        Register('axx1'),
        Register('ax1'),
        Register('ax3'),
        Register('ax2'),
        Register('ix3'),
        Register('ix2'),
        Register('ix1'),
        Register('ITREMAP_interf_itmskl', False),
        Register('ITREMAP_interf_itmskl', False),
        Register('pl'),
        Register('ph'),
        Register('ext6', False),
        Register('ext45', False),
        Register('ext23', False),
        Register('ext01', False),
        Register('r6l'),
        Register('r6h'),
        Register('r5l'),
        Register('r5h'),
        Register('r4l'),
        Register('r4h'),
        Register('r3l'),
        Register('r3h'),
        Register('r2l'),
        Register('r2h'),
        Register('r1l'),
        Register('r1h'),
        Register('r0l'),
        Register('sp_other'),
        Register('sp1'),
        Register('sp0'),
        Register('r0h'),
        Register('flag'),
        Register('adctl')
        ]
config = [
                Config('mem_esram.data',
                            [TransformCode('mem_shared_esram_code.mem'),
                             TransformData16('mem_shared_esram_data16.mem'),
                             TransformData24('mem_shared_esram_data24.mem')]),
                ConfigPattern('mem_tcm.data',
                              [TransformData24('mem_{0}_tcm.mem')]),
                ConfigPattern('mem_sdram_code.data',
                              [TransformCode('mem_{0}_code.mem')]),
                ConfigPattern('mem_sdram_data.data',
                              [TransformData16('mem_{0}_data16.mem'),
                               TransformData24('mem_{0}_data24.mem'),
                               ExtractRegisterFile('reg_{0}.reg', regs)]),
                ]

# main #########################################################################################################
if __name__ == '__main__':
    from optparse import OptionParser
    usage = "usage: %prog [options] basedir"
    parser = OptionParser(usage=usage)
    parser.add_option('-x', '--offset', dest='offset', type='int', default=0x0, #base: 0x10000, dsp24 base adress for ddr24
                      help='register file offset in mem_MPC_data.data file (in dsp24)', metavar='OFFSET')
    parser.add_option("-r", "--reverse", dest="reverse", help="reverse endianness", action="store_false", default=True)
    parser.add_option("-d", "--debug", dest="debug", help="debug level", metavar="LEVEL")
    parser.add_option("-v", "--verbose", dest="verbose", action="store_false", default=True, help="be verbose", )

    (options, args) = parser.parse_args()

    if len(args) != 1:
        print 'Base directory not specified\n'
        parser.print_usage()
        sys.exit(1)

    config_logger(logging.DEBUG)  #TODO, logging._levelNames[options.debug]

    base = args[0]
    log.info('Generating coredumps from basedir: {0}'.format(os.path.realpath(base)))
    (mpc, ) = get_info(os.path.realpath(base))
    log.info('Coredump info: mpc={0}'.format(mpc))
    traverse(config, os.path.realpath(base), mpc, offset=options.offset, reverse=options.reverse)
