<?php

// +---------------------------------------------------------------------------+
// | awss3 Plugin                                                              |
// +---------------------------------------------------------------------------+
// | public_html/admin/plugins/awss3/index.php                                 |
// +---------------------------------------------------------------------------+
// | Copyright (C) 2015 MeYan                                                  |
// |                                                                           |
// | Constructed with the Universal Plugin                                     |
// +---------------------------------------------------------------------------+
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

/**
 * Geeklog common function library
 */
require_once ('../../../lib-common.php');

/**
 * Security check to ensure user even belongs on this page
 */
require_once '../../auth.inc.php';

// Uncomment the line below if you need to debug the HTTP variables being passed
// to the script.  This will sometimes cause errors but it will allow you to see
// the data being passed in a POST operation
// COM_debug($_POST);

$display = '';

if (!SEC_hasRights('awss3.admin')) {
    $display .= COM_showMessageText($MESSAGE[29], $MESSAGE[30]);
    $display = COM_createHTMLDocument($display, array('pagetitle' => $MESSAGE[30]));

    // Log attempt to access.log
    COM_accessLog("User {$_USER['username']} tried to illegally access the event administration screen.");
    COM_output($display);
    
    exit;
}

// +---------------------------------------------------------------------------+
// | Get Mode                                                                  |
// +---------------------------------------------------------------------------+
if ( isset($_GET['mode']) ) {
    $mode = COM_applyFilter($_GET['mode']);
} else {
    if ( isset($_POST['mode']) ) {
        $mode = COM_applyFilter($_POST['mode']);
    } else {
        $mode = '';
    }
}

// +---------------------------------------------------------------------------+
// | MAIN                                                                      |
// +---------------------------------------------------------------------------+
switch ($mode) {

  case 'regist':
      AWSS3_upload($_FILES, $_POST);
      break;

  case 'delete':
      AWSS3_delete($_GET['key']);
      break;
      
  case 'config':
      AWSS3_config("");
      break;
      
  case 'save':
      AWSS3_save($_POST);
      break;
      
  case 'get':
      AWSS3_download($_GET['key']);
      break;
      
  case 'cancel':
  case 'list':
  default:
      AWSS3_admindefault("");
      break;
}

?>
