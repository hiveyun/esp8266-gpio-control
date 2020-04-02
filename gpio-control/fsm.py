def gen_button(count):
    for i in range(1, count + 1):
        print('        button{} -(switch{}.toggle)-,'.format(i, i))
        print('        button{}LongPress -(smartconfig.smartconfig)-,'.format(i))

def main(count):
    print('fsm {')
    print('    * power-on --> idle,')
    print('    idle [')
    gen_button(count)
    loops = []
    for i in range(1, count + 1):
        loops.append('button{}.loop'.format(i))
        loops.append('switch{}.loop'.format(i))
    print('        loop -(network.loop, blinker.loop, {})-,'.format(', '.join(loops)))
    print('    ],')
    print('}')

if __name__ == '__main__':
    import sys
    main(int(sys.argv[1]))
