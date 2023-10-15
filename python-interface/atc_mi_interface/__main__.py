#############################################################################
# __main__.py: atc_mi_interface module
#############################################################################

import argparse
import sys
from . import atc_mi_advertising
from . import atc_mi_config
from . import atc_mi_format_test
from .__version__ import __version__

def main():
    parser = argparse.ArgumentParser(
        prog='atc_mi_interface',
        description='Subsequent options must follow, related to the selected '
            'tool. The first argument is the option which selects the tool '
            '(advertising, config, or test) and must be separated from the '
            'subsequent tool options, to be placed in other arguments.',
        epilog='atc_mi_interface tools')
    config_group = parser.add_mutually_exclusive_group(required=True)
    config_group.add_argument(
        '-a',
        '--advertising',
        dest='advertising',
        action='store_true',
        help='Run the atc_mi_advertising tool')
    config_group.add_argument(
        '-c',
        "--config",
        dest='config',
        action='store_true',
        help="Run the atc_mi_config tool")
    config_group.add_argument(
        '-t',
        "--test",
        dest='test',
        action='store_true',
        help="Run the atc_mi_format_test tool")
    parser.add_argument(
        '-H',
        "--help-option",
        dest='subhelp',
        action='store_true',
        help="Invoke the specific help of the selected tool")
    parser.add_argument(
        '-V',
        "--version",
        dest='version',
        action='store_true',
        help="Print version and exit")

    args, unknown = parser.parse_known_args()
    if args.version:
        print(f'atc_mi_interface version {__version__}')
        sys.exit(0)
    if len(sys.argv) > 1:
        sys.argv.pop(1)
    if args.subhelp:
        sys.argv.append("--help")
    if args.advertising:
        atc_mi_advertising.main()
    if args.config:
        atc_mi_config.main()
    if args.test:
        atc_mi_format_test.main()


if __name__ == "__main__":
    main()
