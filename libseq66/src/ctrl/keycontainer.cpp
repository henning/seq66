/*
 *  This file is part of seq66.
 *
 *  seq66 is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  seq66 is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with seq66; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/**
 * \file          keycontainer.cpp
 *
 *  This module declares/defines a container for key-ordinals and MIDI
 *  operation information.
 *
 * \library       seq66 application
 * \author        Chris Ahlstrom
 * \date          2018-11-18
 * \updates       2020-04-09
 * \license       GNU GPLv2 or above
 *
 */

#include <iomanip>                      /* std::setw manipulator            */
#include <iostream>                     /* std::cerr                        */

#include "cfg/settings.hpp"             /* seq66::rc() rcsettings getter    */
#include "ctrl/keycontainer.hpp"        /* seq66::keycontainer class        */
#include "ctrl/keymap.hpp"              /* seq66::qt_keyname_ordinal()      */

/*
 *  Do not document a namespace; it breaks Doxygen.
 */

namespace seq66
{

/**
 *  This default constructor creates a "zero" object.  Every member is
 *  either false or some other form of zero.
 */

keycontainer::keycontainer () :
    m_container         (),
    m_container_name    ("Default keys"),
    m_pattern_keys      (),
    m_mute_keys         (),
    m_loaded_from_rc    (false)
{
    add_defaults();
}

/**
 *  This constructor assigns the basic values of control name, number, and
 *  action code.  The rest of the members can be set via the set() function.
 */

keycontainer::keycontainer (const std::string & name) :
    m_container         (),
    m_container_name    (name),
    m_pattern_keys      (),
    m_mute_keys         (),
    m_loaded_from_rc    (false)
{
    add_defaults();
}

/**
 *  Adds the keystroke ordinal and it corresponding key control to the key
 *  container.
 *
 * \param ordinal
 *      Provides the keystroke value (see the keymap.cpp module).
 *
 * \param op
 *      Provides the key-control operation to be triggered by this keystroke.
 *
 * \return
 *      Returns true if the container size increased by 1.
 */

bool
keycontainer::add (ctrlkey ordinal, const keycontrol & op)
{
    bool result = false;
    auto sz = m_container.size();
    auto p = std::make_pair(ordinal, op);   /* std::pair<int, keycontrol>   */
    (void) m_container.insert(p);
    result = m_container.size() == (sz + 1);
    if (result)
    {
#if defined SEQ66_PLATFORM_DEBUG_TMI
        std::string keyname = op.key_name();
        std::cout
            << "Inserted pattern key ordinal #" << std::setw(3) << ordinal
            << " : '" << keyname << "'" << std::endl
                ;
#endif
    }
    else
    {
        /*
         * Currently the "~" key (ordinal #126) is used only as a placeholder
         * for loop-control entries above the normal 4x8 sets.  We don't want
         * to spam the user on the console with 30+ of these reports.
         */

        if (ordinal != 126)
        {
            std::string tag = is_invalid_ordinal(ordinal) ?
                "Invalid" : "Duplicate" ;

            std::cerr
                << tag << " key (#" << ordinal
                << " = '" << qt_ordinal_keyname(ordinal) << "')"
                << " for '" << op.name()
                << "' Category " << op.category_name()
                << std::endl
                ;
        }
    }
    return result;
}

/**
 *  Add to a map of key-names (normally a single character) keyed by slot
 *  numbers.  These are useful in updating the live windows.  Only the
 *  pattern-control mappings are added to this container.
 *
 * \param op
 *      Provides the key-control operation to be triggered by this keystroke.
 *      This item provides the pattern offset for the slot, and the name of the
 *      keystroke that toggles the slot.
 *
 * \return
 *      Returns true if the container size increased by 1.
 */

bool
keycontainer::add_slot (const keycontrol & op)
{
    bool result = false;
    int keyslot = op.control_code();            /* pattern offset       */
    std::string keyname = op.key_name();
    auto sz = m_pattern_keys.size();
    auto p = std::make_pair(keyslot, keyname);
    (void) m_pattern_keys.insert(p);
    result = m_pattern_keys.size() == (sz + 1);
    if (result)
    {
#if defined SEQ66_PLATFORM_DEBUG_TMI
        std::cout
            << "Inserted pattern slot #" << std::setw(3) << keyslot
            << " : '" << keyname << "'" << std::endl
            ;
#endif
    }
    else
    {
        std::cerr
            << "Duplicate pattern slot #" << std::setw(3) << keyslot
            << " : '" << keyname << "'" << std::endl
            ;
    }
    return result;
}

/**
 *  Add to a map of key-names (normally a single character) keyed by mute
 *  numbers.  These are useful in updating the mute-master window.  Only the
 *  mute-group-control mappings are added to this container.
 *
 * \param op
 *      Provides the key-control operation to be triggered by this keystroke.
 *      This item provides the mute offset for the mute group, and the name of
 *      the keystroke that activates the mute group.
 *
 * \return
 *      Returns true if the container size increased by 1.
 */

bool
keycontainer::add_mute (const keycontrol & op)
{
    bool result = false;
    int keyslot = op.control_code();            /* pattern offset       */
    std::string keyname = op.key_name();
    auto sz = m_mute_keys.size();
    auto p = std::make_pair(keyslot, keyname);
    (void) m_mute_keys.insert(p);
    result = m_mute_keys.size() == (sz + 1);
    if (result)
    {
#if defined SEQ66_PLATFORM_DEBUG_TMI
        std::cout
            << "Inserted mute slot #" << std::setw(3) << keyslot
            << " : '" << keyname << "'" << std::endl
            ;
#endif
    }
    else
    {
        std::cerr
            << "Duplicate mute slot #" << std::setw(3) << keyslot
            << " : '" << keyname << "'" << std::endl
            ;
    }
    return result;
}

/**
 *  Looks up the key-control object matching the given keystroke ordinal, as
 *  returned by qt_modkey_ordinal() or qt_keyname_ordinal().
 */

const keycontrol &
keycontainer::control (ctrlkey ordinal) const
{
    static keycontrol sm_keycontrol_dummy;
    const auto & cki = m_container.find(ordinal);
    return (cki != m_container.end()) ? cki->second : sm_keycontrol_dummy;
}

/**
 *  If verbose is set, shows the contents of the keycontainer.
 */

void
keycontainer::show () const
{
    int index = 0;
    std::cout << "Key container size: " << m_container.size() << std::endl;
    std::cout << "Index Key Name" << std::endl;
    for (const auto & kp : m_container)
    {
        std::cout
            << "[" << std::setw(3) << std::right << index << "] ("
            <<        std::setw(3) << std::right << kp.first << ") "
            ;

        kp.second.show();
        ++index;
    }
}

/**
 *  We had to put the static vectors inside this function because they were not
 *  initialized in time for their usage.  Odd.
 */

void
keycontainer::add_defaults ()
{
    static std::vector<std::string> s_keys_pattern =
    {
        "1", /*  0 */ "q", /*  1 */ "a", /*  2 */ "z", /*  3 */
        "2", /*  4 */ "w", /*  5 */ "s", /*  6 */ "x", /*  7 */
        "3", /*  8 */ "e", /*  9 */ "d", /* 10 */ "c", /* 11 */
        "4", /* 12 */ "r", /* 13 */ "f", /* 14 */ "v", /* 15 */
        "5", /* 16 */ "t", /* 17 */ "g", /* 18 */ "b", /* 19 */
        "6", /* 20 */ "y", /* 21 */ "h", /* 22 */ "n", /* 23 */
        "7", /* 24 */ "u", /* 25 */ "j", /* 26 */ "m", /* 27 */
        "8", /* 28 */ "i", /* 29 */ "k", /* 30 */ ",", /* 31 */
    };
    static std::vector<std::string> s_keys_mute_group =
    {
        "!", /*  0 */ "Q", /*  1 */ "A", /*  2 */ "Z", /*  3 */
        "@", /*  4 */ "W", /*  5 */ "S", /*  6 */ "X", /*  7 */
        "#", /*  8 */ "E", /*  9 */ "D", /* 10 */ "C", /* 11 */
        "$", /* 12 */ "R", /* 13 */ "F", /* 14 */ "V", /* 15 */
        "%", /* 16 */ "T", /* 17 */ "G", /* 18 */ "B", /* 19 */
        "^", /* 20 */ "Y", /* 21 */ "H", /* 22 */ "N", /* 23 */
        "&", /* 24 */ "U", /* 25 */ "J", /* 26 */ "M", /* 27 */
        "*", /* 28 */ "I", /* 29 */ "K", /* 30 */ "<", /* 31 */
    };

    /*
     *  Indicates the default keystroke and action status of a particular
     *  automation keystroke operation.  Matches the slot enum class.
     */

    static std::vector<keydefault> s_keys_automation =
    {
        { "'",         automation::action::on      },  //  0 bpm_up
        { ";",         automation::action::on      },  //  1 bpm_dn
        { "]",         automation::action::on      },  //  2 ss_up
        { "[",         automation::action::on      },  //  3 ss_dn
        { "KP_Home",   automation::action::on      },  //  4 mod_replace
        { "Ins",       automation::action::on      },  //  5 mod_snapshot
        { "o",         automation::action::on      },  //  6 mod_queue
        { "`",         automation::action::on      },  //  7 mod_gmute
        { "l", /*el*/  automation::action::on      },  //  8 mod_glearn
        { "Home",      automation::action::on      },  //  9 play_ss
        { ".",         automation::action::toggle  },  // 10 playback (pause)
        { "P",         automation::action::on      },  // 11 song_record
        { "BS",        automation::action::on      },  // 12 solo
        { "KP_/",      automation::action::on      },  // 13 thru
        { "PageUp",    automation::action::on      },  // 14 bpm_page_up
        { "PageDn",    automation::action::on      },  // 15 bpm_page_dn
        { "KP_.",      automation::action::on      },  // 16 ss_set
        { "KP_*",      automation::action::on      },  // 17 record
        { "KP_-",      automation::action::on      },  // 18 quan_record
        { "KP_+",      automation::action::on      },  // 19 reset_seq
        { "|",         automation::action::on      },  // 20 mod_oneshot
        { "F6",        automation::action::on      },  // 21 FF
        { "F5",        automation::action::on      },  // 22 rewind
        { "F1",        automation::action::on      },  // 23 top (beginning)
        { "F2",        automation::action::on      },  // 24 playlist (next)
        { "F3",        automation::action::on      },  // 25 playlist_song (next)
        { "F9",        automation::action::on      },  // 26 tap_bpm
        { "Space",     automation::action::on      },  // 27 start [not " "!]
        { "Esc",       automation::action::on      },  // 28 stop
        { "KP_Ins",    automation::action::on      },  // 29 mod_snapshot_2
        { "F8",        automation::action::on      },  // 30 toggle_mutes
        { "F7",        automation::action::on      },  // 31 song_pointer
        { "\\",        automation::action::on      },  // 32 keep_queue
        { "/",         automation::action::off     },  // 33 slot_shift
        { "0",         automation::action::on      },  // 34 mutes_clear
        { "Null_f1",   automation::action::off     },  // 35 reserved_35
        { "=",         automation::action::on      },  // 36 pattern_edit
        { "-",         automation::action::on      },  // 37 event_edit
        { "F10",       automation::action::on      },  // 38 song_mode
        { "F11",       automation::action::on      },  // 39 toggle_jack
        { "F12",       automation::action::on      },  // 40 menu_mode
        { "F4",        automation::action::on      },  // 41 follow_transport
        { "~",         automation::action::on      },  // 42 panic
        { "Null_f9",   automation::action::off     },  // 43 reserved_43
        { "Null_fa",   automation::action::off     },  // 44 reserved_44
        { "Null_fb",   automation::action::off     },  // 45 reserved_45
        { "Null_fc",   automation::action::off     },  // 46 reserved_46
        { "Null_fd",   automation::action::off     },  // 47 reserved_47
        { "Null_fe",   automation::action::off     },  // 48 reserved_48
        { "Null_ff",   automation::action::off     },  // -- maximum
        { "Null_g0",   automation::action::toggle  },  // loop/pattern function
        { "Null_g1",   automation::action::toggle  },  // mute_group function
        { "Null_g3",   automation::action::none    }   // automation functions
    };

    clear();

    /*
     *  Pattern-control keys.
     */

    std::string tagprefix{"Loop "};             /* shorter than "Pattern"   */
    for (int seq = 0; seq < int(s_keys_pattern.size()); ++seq)
    {
        std::string nametag = tagprefix + std::to_string(seq);
        keycontrol kc
        (
            nametag,
            s_keys_pattern[seq],                /* provides the key name    */
            automation::category::loop,
            automation::action::toggle, automation::slot::loop, seq
        );
        ctrlkey ordinal = qt_keyname_ordinal(s_keys_pattern[seq]);
        if (! add(ordinal, kc))
            break;

        if (! add_slot(kc))                     /* provides slot number     */
            break;
    }

    /*
     * Mute-group-control keys.
     */

    tagprefix = "Mute ";
    for (int group = 0; group < int(s_keys_mute_group.size()); ++group)
    {
        std::string nametag = tagprefix + std::to_string(group);
        keycontrol kc
        (
            nametag,
            s_keys_mute_group[group],           /* provides the key name    */
            automation::category::mute_group,
            automation::action::toggle, automation::slot::mute_group, group
        );
        ctrlkey ordinal = qt_keyname_ordinal(s_keys_mute_group[group]);
        if (! add(ordinal, kc))
            break;

        if (! add_mute(kc))                     /* provides mute number     */
            break;
    }

    /*
     * Automation-control keys. Any way to grab the real name from Qt?
     */

    tagprefix = "Auto ";
    automation::category c = automation::category::automation;
    for (int auslot = 0; auslot < int(s_keys_automation.size()); ++auslot)
    {
        automation::slot s = static_cast<automation::slot>(auslot);
        automation::action a = s_keys_automation[auslot].kd_action;
        std::string nametag = opcontrol::slot_name(s);
        std::string keyname = s_keys_automation[auslot].kd_name;
        ctrlkey ordinal = qt_keyname_ordinal(keyname);
        if (is_invalid_ordinal(ordinal))
        {
            /* not sure we want to see a message here */
        }
        else
        {
            keycontrol kc(nametag, keyname, c, a, s, auslot);
            if (! add(ordinal, kc))     /*  && int(ordinal) != (-1))    */
                break;
        }
    }
    m_loaded_from_rc = false;
}

}           // namespace seq66

/*
 * keycontainer.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

