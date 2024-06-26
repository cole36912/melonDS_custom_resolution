/*
    Copyright 2021-2022 melonDS team

    This file is part of melonDS.

    melonDS is free software: you can redistribute it and/or modify it under
    the terms of the GNU General Public License as published by the Free
    Software Foundation, either version 3 of the License, or (at your option)
    any later version.

    melonDS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with melonDS. If not, see http://www.gnu.org/licenses/.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <QApplication>
#include <QCommandLineParser>
#include <QStringList>

#include "CLI.h"

namespace CLI
{

CommandLineOptions* ManageArgs(QApplication& melon)
{
    QCommandLineParser parser;
    parser.addHelpOption();

    parser.addPositionalArgument("nds", "Nintendo DS ROM (or an archive file which contains it) to load into Slot-1");
    parser.addPositionalArgument("gba", "GBA ROM (or an archive file which contains it) to load into Slot-2");

    parser.addOption(QCommandLineOption({"b", "boot"}, "Whether to boot firmware on startup. Defaults to \"auto\" (boot if NDS rom given)", "auto/always/never", "auto"));
    parser.addOption(QCommandLineOption({"f", "fullscreen"}, "Start melonDS in fullscreen mode"));
    
#ifdef ARCHIVE_SUPPORT_ENABLED
    parser.addOption(QCommandLineOption({"a", "archive-file"}, "Specify file to load inside an archive given (NDS)", "rom"));
    parser.addOption(QCommandLineOption({"A", "archive-file-gba"}, "Specify file to load inside an archive given (GBA)", "rom"));
#endif

    parser.process(melon);

    CommandLineOptions* options = new CommandLineOptions;

    options->fullscreen = parser.isSet("fullscreen");
    
    QStringList posargs = parser.positionalArguments();
    switch (posargs.size())
    {
        default:
            printf("Too many positional arguments; ignoring 3 onwards\n");
        case 2:
            options->gbaRomPath = QStringList(posargs[1]);
        case 1:
            options->dsRomPath = QStringList(posargs[0]);
        case 0:
            break;
    }

    QString bootMode = parser.value("boot");
    if (bootMode == "auto")
    {
        options->boot = posargs.size() > 0;
    } 
    else if (bootMode == "always")
    {
        options->boot = true;
    }
    else if (bootMode == "never")
    {
        options->boot = false;
    }
    else
    {
        printf("ERROR: -b/--boot only accepts auto/always/never as arguments\n");
        exit(1);
    }

#ifdef ARCHIVE_SUPPORT_ENABLED
    if (parser.isSet("archive-file"))
    {
        if (options->dsRomPath.isEmpty())
        {
            options->errorsToDisplay += "Option -a/--archive-file given, but no archive specified!";
        }
        else
        {
            options->dsRomPath += parser.value("archive-file");
        }
    } 
    else if (!options->dsRomPath.isEmpty())
    {
        //TODO-CLI: try to automatically find ROM
        QStringList paths = options->dsRomPath[0].split("|");
        if (paths.size() >= 2)
        {
            printf("Warning: use the a.zip|b.nds format at your own risk!\n");
            options->dsRomPath = paths;
        }
    }

    if (parser.isSet("archive-file-gba"))
    {
        if (options->gbaRomPath.isEmpty())
        {
            options->errorsToDisplay += "Option -A/--archive-file-gba given, but no archive specified!";
        }
        else
        {
            options->gbaRomPath += parser.value("archive-file-gba");
        }
    }
    else if (!options->gbaRomPath.isEmpty())
    {
        //TODO-CLI: try to automatically find ROM
        QStringList paths = options->gbaRomPath[0].split("|");
        if (paths.size() >= 2)
        {
            printf("Warning: use the a.zip|b.gba format at your own risk!\n");
            options->gbaRomPath = paths;
        }
    }
#endif

    return options;
}

}