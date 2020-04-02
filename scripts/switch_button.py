import os
import sys

in_path = os.path.abspath(sys.path[0]+"/../in")

def main(count):
    for i in range(1, count + 1):
        pin=os.getenv('BUTTON_PIN_{}'.format(i), None)
        if not pin:
            print('Error: you must set env BUTTON_PIN_{}'.format(i))


        os.system("sed -e 's/BUTTON_NAME/{}/g' -e 's/BUTTON_PIN/{}/g' {}/button.cpp.in > button{}.cpp".format(i, pin, in_path, i))

        pin=os.getenv('SWITCH_PIN_{}'.format(i), None)
        if not pin:
            print('Error: you must set env SWITCH_PIN_{}'.format(i))

        os.system("sed -e 's/SWITCH_NAME/{}/g' -e 's/SWITCH_PIN/{}/g' {}/switch.cpp.in > switch{}.cpp".format(i, pin, in_path, i))

if __name__ == '__main__':
    main(int(sys.argv[1]))
