<?php

// +---------------------------------------------------------------------------+
// | awss3 Plugin                                                              |
// +---------------------------------------------------------------------------+
// | mysql_install.php                                                         |
// +---------------------------------------------------------------------------+
// | Copyright (C) 2015 MeYan                                                  |
// |                                                                           |
// | Constructed with the Universal Plugin                                     |
// +---------------------------------------------------------------------------|
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
// | along with this program; if not, write to the Free Software               |
// | Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA|
// |                                                                           |
// +---------------------------------------------------------------------------|

$_SQL[] = "CREATE TABLE {$_TABLES['awss3']} (
  iamkey varchar(80) NOT NULL default 'MyIAMKey',
  iamskey varchar(80) NOT NULL default 'MyIAMSecretKey',
  region varchar(16) NOT NULL default 'ap-northeast-1',
  bucket varchar(32) NOT NULL default 'MyBucket',
  acl varchar(16) NOT NULL default 'private',
  PRIMARY KEY (iamkey)
) ENGINE=MyISAM;";

?>
