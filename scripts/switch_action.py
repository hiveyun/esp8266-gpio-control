def combine(fn, switch_actions):
    with open(fn, 'r') as f:
        data = f.read()

    return data.replace('{SWITCH_ACTIONS}', '\n'.join(switch_actions))

def gen_if_else(n, action):
    codes = []
    for i in range(1, n + 1):
        if i == 1:
            codes.append('    if (index == 1) {')
        else:
            codes.append('    } else if (index ==' + str(i) + ') {')
        codes.append(action(i))

    return '\n'.join(codes)

def gen_state(i):
    return '        sprintf(stateName, "%s", switch{}_Current_state_name());'.format(i)

def gen_switch_action(action):
    def gen(i):
        return '        switch{}_{}(NULL);'.format(i, action)

    return gen

def gen_switch_func(count, func_name, state_name):
    codes = []
    codes.append('''void {}(int index) $$
    // Prepare switchs JSON payload string
    stateName[0] = '\\0';
    sprintf(stateName, "%s", "{}");
'''.format(
        func_name, state_name).replace('$$', '{'))
    codes.append(gen_if_else(count, gen_switch_action(state_name)))
    codes.append('''
    } else {
        stateName[0] = '\\0';
        sprintf(stateName, "Switch %d not exists.", index);
    }
    genSwitchState(index, stateName);
}
''')
    return ''.join(codes)

def main(fn, count):
    switch_actions = []
    switch_actions.append('''void getSwitchState(int index) {
    // Prepare switchs JSON payload string
    stateName[0] = '\\0';''')
    switch_actions.append(gen_if_else(count, gen_state))
    switch_actions.append('''    } else {
        sprintf(stateName, "Switch %d not exists.", index);
    }
    genSwitchState(index, stateName);
}
''')
    switch_actions.append(gen_switch_func(count, 'setSwitchOn', 'on'))
    switch_actions.append(gen_switch_func(count, 'setSwitchOff', 'off'))

    print(combine(fn, switch_actions))

if __name__ == '__main__':
    import sys
    main(sys.argv[1], int(sys.argv[2]))
