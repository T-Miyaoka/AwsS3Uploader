<?php

/* Reminder: always indent with 4 spaces (no tabs). */
// +---------------------------------------------------------------------------+
// | Empty Plugin 0.0.1                                                        |
// +---------------------------------------------------------------------------+
// | autoinstall.php                                                           |
// |                                                                           |
// | This file provides helper functions for the automatic plugin install.     |
// +---------------------------------------------------------------------------+
// | Copyright (C) 2018 by the following authors:                              |
// |                                                                           |
// | Authors: MeYan                                                            |
// +---------------------------------------------------------------------------+
// |                                                                           |
// | This program is free software; you can redistribute it and/or             |
// | modify it under the terms of the GNU General Public License               |
// | as published by the Free Software Foundation; either version 2            |
// | of the License, or (at your option) any later version.                    |
// |                                                                           |
// | This program is distributed in the hope that it will be useful,           |
// | but WITHOUT ANY WARRANTY; without even the implied warranty of            |
// | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             |
// | GNU General Public License for more details.                              |
// |                                                                           |
// | You should have received a copy of the GNU General Public License         |
// | along with this program; if not, write to the Free Software Foundation,   |
// | Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.           |
// |                                                                           |
// +---------------------------------------------------------------------------+
if (strpos(strtolower($_SERVER['PHP_SELF']), 'autoinstall.php') !== FALSE) {
    die('This file cannot be used on its own.');
}
/**
* Autoinstall API functions for the Empty plugin
*
* @package Empty
*/

/**
* Plugin autoinstall function
*
* @param    string  $pi_name    Plugin name
* @return   array               Plugin information
*
*/
function plugin_autoinstall_awss3($pi_name) {
    return array(
        'info'                => array(
            'pi_name'         => 'awss3',
            'pi_display_name' => 'AwsS3 Uploader',
            'pi_version'      => '0.0.1',
            'pi_gl_version'   => '2.0.0',
            'pi_homepage'     => 'http://www.happa.bz/',
        ),
        'tables'              => array('awss3'),
        'groups'              => array(
            'AwsS3 Admin'     => 'Users in this group can administer the AwsS3 Uploder plugin',
        ),
        'features'            => array(
            'awss3.admin'     => 'Access to AwsS3 Uploder plugin editor',
        ),
        'mappings'            => array(
            'awss3.admin'     => array('AwsS3 Admin'),
        ),
        'requires'            => array(
            array(
            'db'              => 'mysql',
            'version'         => '4.1'
            ),
        ),
    );
}

?>

