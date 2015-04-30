/****************************************************************************
 * This file is part of midikey.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL2+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#include <QtCore/QDebug>
#include <QtGui/QtEvents>

#include "keyfilter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <termios.h>
#include <signal.h>

KeyFilter::KeyFilter(QObject *parent)
    : QObject(parent)
{
    int result = 0;
    char name[256] = "Unknown";
    char *device = "/dev/input/event0";

    int fevdev = open(device, O_RDONLY);
    if (fevdev == -1) {
        printf("Failed to open event device.\n");
        exit(1);
    }

    result = ioctl(fevdev, EVIOCGNAME(sizeof(name)), name);
    qDebug("Reading from: %s (%s)", device, name);

    result = ioctl(fevdev, EVIOCGRAB, 0);

    m_notifier = new QSocketNotifier(fevdev, QSocketNotifier::Read, this);
    connect(m_notifier, &QSocketNotifier::activated, this, [this](int fd) {
        struct ::input_event buffer[32];
        int n = 0;

        forever {
            int result = read(fd, reinterpret_cast<char *>(buffer) + n, sizeof(buffer) - n);
            if (result == 0) {
                qWarning() << "EOF";
                return;
            } else if (result < 0) {
                if (errno != EINTR && errno != EAGAIN) {
                    qErrnoWarning(errno, "Could not read from input device");
                    return;
                }
            } else {
                n += result;
                if (n % sizeof(buffer[0]) == 0)
                    break;
            }
        }

        n /= sizeof(buffer[0]);

        for (int i = 0; i < n; ++i) {
            if (buffer[i].type != EV_KEY)
                continue;

            quint16 code = buffer[i].code;
            qint32 value = buffer[i].value;
            bool pressed = value != 0;
            bool autorepeat = value == 2;

            if (pressed && autorepeat)
                continue;

            if (pressed)
                Q_EMIT keyPressed(code);
            else
                Q_EMIT keyReleased(code);
        }
    });
}
