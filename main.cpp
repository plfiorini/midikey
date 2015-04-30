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

#include <QCoreApplication>
#include <QtCore/QDebug>

#include "QMidi/QMidiOut.h"
#include "keyfilter.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QMidiOut *midiOut = new QMidiOut;
    if (!midiOut->connect(QStringLiteral("128:0")))
        qFatal("Errore di connessione alla porta MIDI 128:0");

    midiOut->setInstr(0, 0);

    KeyFilter *keyFilter = new KeyFilter(midiOut);
    QObject::connect(keyFilter, &KeyFilter::keyPressed, [midiOut](quint16 keyCode) {
        midiOut->noteOn(keyCode, 0, 127);
    });

    QObject::connect(&a, &QCoreApplication::aboutToQuit, [&] {
        midiOut->stopAll();
    });

    return a.exec();
}
