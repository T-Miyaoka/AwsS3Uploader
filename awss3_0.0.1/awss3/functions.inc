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
if (stripos($_SERVER['PHP_SELF'], basename(__FILE__)) !== false) {
    die ('This file can not be used on its own.');
}
/**
* Implementation of the Plugin API for the Empty plugin
*
* @package Empty
*/

// Install file Include

$installfile = $_CONF['path'] . 'plugins/awss3/autoinstall.php';
if (file_exists($installfile)) {
    require_once $installfile;
    $_AWSS3_CONF = plugin_autoinstall_awss3('');
}

// Language file Include
$langfile = $_CONF['path'] . 'plugins/awss3/language/' . $_CONF['language'] . '.php';
if (file_exists($langfile)) {
    include_once $langfile;
}

// Table info
global $_DB_table_prefix, $_TABLES;
$_TABLES['awss3'] = $_DB_table_prefix . 'awss3';

/*******************************************************************************
* Include AWS Lib                                                              |
********************************************************************************/
$awsfile = $_CONF['path'] . 'plugins/awss3/aws/aws-autoloader.php';
if (file_exists($awsfile)) {
    require_once $awsfile;
}
use Guzzle\Http\EntityBody;
use Aws\Common\Enum\Region;
use Aws\S3\S3Client;
use Aws\S3\Exception\S3Exception;

// +---------------------------------------------------------------------------+
// | Geeklog Plugin API Implementation                                         |
// +---------------------------------------------------------------------------+

/**
* Return the version for this plugin
*
* @return string VersionNo
*/
function plugin_chkVersion_awss3() {
    global $_AWSS3_CONF;
    return $_AWSS3_CONF['info']['pi_version'];
}

/**
* Upgrade the plugin
*
* @return boolean true (= success)
*/
function plugin_upgrade_awss3() {
    global $_TABLES, $_AWSS3_CONF;
    
    $sql = "UPDATE {$_TABLES['plugins']} SET "
    . "pi_version = '{$_AWSS3_CONF['info']['pi_version']}',"
    . "pi_gl_version = '{$_AWSS3_CONF['info']['pi_gl_version']}' "
    . "WHERE pi_name = '{$_AWSS3_CONF['info']['pi_name']}'";
    DB_query($sql);

    return True;
}

/**
* Automatic uninstall function for plugins
*
* @return array
*/
function plugin_autouninstall_awss3() {
    global $_AWSS3_CONF;
    
    return array(
        'tables'     => $_AWSS3_CONF['tables'],
        'groups'     => $_AWSS3_CONF['groups'],
        'features'   => $_AWSS3_CONF['features'],
    );
}

/**
* This will put an option for the plugin in the command and control block on
* moderation.php
*
* @return   array   Array containing (plugin name, admin url, url of plugin icon)
*/
function plugin_cclabel_awss3() {
    global $_CONF, $LANG_JTK;
    
    $pi_name   = AWSS3_str('admin_label');
    $admin_url = $_CONF['site_admin_url'] . '/plugins/awss3/index.php';
    $icon_url  = $_CONF['site_url'] . '/awss3/images/Storage_AmazonS3.png';

    return array($pi_name, $admin_url, $icon_url);
}

/**
* Returns the administrative option for this plugin
*
* @return   array   Array containing (plugin name, plugin admin url, # of items
*                   in plugin or '')
*/
function plugin_getadminoption_awss3() {
    global $_USER, $_CONF, $_TABLES, $LANG_JTK;

    if (SEC_hasRights('awss3.admin')){
        $pi_name   = AWSS3_str('admin_label');
        $admin_url = $_CONF['site_admin_url'] . '/plugins/awss3/index.php';
        $num_items  = "";

        return array($pi_name, $admin_url, $num_items);
    }
}

/**
 * Returns the items for this plugin that should appear on the main menu
 * NOTE: this MUST return the url/value pairs in the following format
 * $<arrayname>[<label>] = <url>
 */
function plugin_getmenuitems_awss3() {
    global $_CONF;
    $menuitems[AWSS3_str('admin_label')] = $_CONF['site_url'] . '/awss3/index.php';
    return $menuitems;
}

// -------------------------------------------------------------------------------------------------------------- //
// AWS S3 Functions                                                                                               //
// -------------------------------------------------------------------------------------------------------------- //
/*
* AWSS3_str
*/
function AWSS3_str($str) {
    global $LANG_AWSS3;
    return isset($LANG_AWSS3[$str]) ? $LANG_AWSS3[$str] : '(undefined)';
}

/*
* AWSS3_save
*/
function AWSS3_save($config) {
    global $_TABLES;

    $sql  = "";
    $sql .= "delete from {$_TABLES['awss3']}";
    DB_query($sql);

    $sql  = "";
    $sql .= "insert into {$_TABLES['awss3']} ";
    $sql .= "(iamkey, iamskey, region, bucket, acl) ";
    $sql .= "values ";
    $sql .= "('{$config['iamkey']}','{$config['iamskey']}','{$config['region']}','{$config['bucket']}','{$config['acl']}')";
    $result = DB_query($sql);
    
    if($result = True) {
        AWSS3_config("設定を変更しました。");
    }
    else {
        AWSS3_config("設定を変更できませんでした。");
    }
}

/*
* AWSS3_config
*/
function AWSS3_config($msg) {
    global $_CONF;

    $config = AWSS3_getconfig();
    $content = "";

    $T = new Template($_CONF['path'] . 'plugins/awss3/templates');
    
    if($msg == "") {
        $T->set_file(array (
            'topmenu' => 'topmenu.thtml',
            'config'  => 'config.thtml',
        ));

        $T->set_var(array(
            'lang_piname'               => AWSS3_str('admin_label'),
            'icon_url'                  => $_CONF['site_url'] . '/awss3/images/Storage_AmazonS3.png',
            'iamkey'                    => $config['iamkey'],
            'iamskey'                   => $config['iamskey'],
            'bucket'                    => $config['bucket'],
            'us-east-1'                 => $config['us-east-1'],
            'us-east-2'                 => $config['us-east-2'],
            'us-west-1'                 => $config['us-west-1'],
            'us-west-2'                 => $config['us-west-2'],
            'ca-central-1'              => $config['ca-central-1'],
            'eu-central-1'              => $config['eu-central-1'],
            'eu-west-1'                 => $config['eu-west-1'],
            'eu-west-2'                 => $config['eu-west-2'],
            'eu-west-3'                 => $config['eu-west-3'],
            'ap-northeast-1'            => $config['ap-northeast-1'],
            'ap-northeast-2'            => $config['ap-northeast-2'],
            'ap-northeast-3'            => $config['ap-northeast-3'],
            'ap-southeast-1'            => $config['ap-southeast-1'],
            'ap-southeast-2'            => $config['ap-southeast-2'],
            'ap-south-1'                => $config['ap-south-1'],
            'sa-east-1'                 => $config['sa-east-1'],
            'private'                   => $config['private'],
            'public-read'               => $config['public-read'],
            'public-read-write'         => $config['public-read-write'],
            'authenticated-read'        => $config['authenticated-read'],
            'aws-exec-read'             => $config['aws-exec-read'],
            'bucket-owner-read'         => $config['bucket-owner-read'],
            'bucket-owner-full-control' => $config['bucket-owner-full-control'],
        ));
    }
    else {
        $T->set_file(array (
            'blockheader-message' => 'blockheader-message.thtml',
            'topmenu'             => 'topmenu.thtml',
            'config'              => 'config.thtml',
        ));

        $T->set_var(array(
            'msg_title'                 => 'AWS S3 Uploder Message',
            'msg_text'                  => $msg,
            'lang_piname'               => AWSS3_str('admin_label'),
            'icon_url'                  => $_CONF['site_url'] . '/awss3/images/Storage_AmazonS3.png',
            'iamkey'                    => $config['iamkey'],
            'iamskey'                   => $config['iamskey'],
            'bucket'                    => $config['bucket'],
            'us-east-1'                 => $config['us-east-1'],
            'us-east-2'                 => $config['us-east-2'],
            'us-west-1'                 => $config['us-west-1'],
            'us-west-2'                 => $config['us-west-2'],
            'ca-central-1'              => $config['ca-central-1'],
            'eu-central-1'              => $config['eu-central-1'],
            'eu-west-1'                 => $config['eu-west-1'],
            'eu-west-2'                 => $config['eu-west-2'],
            'eu-west-3'                 => $config['eu-west-3'],
            'ap-northeast-1'            => $config['ap-northeast-1'],
            'ap-northeast-2'            => $config['ap-northeast-2'],
            'ap-northeast-3'            => $config['ap-northeast-3'],
            'ap-southeast-1'            => $config['ap-southeast-1'],
            'ap-southeast-2'            => $config['ap-southeast-2'],
            'ap-south-1'                => $config['ap-south-1'],
            'sa-east-1'                 => $config['sa-east-1'],
            'private'                   => $config['private'],
            'public-read'               => $config['public-read'],
            'public-read-write'         => $config['public-read-write'],
            'authenticated-read'        => $config['authenticated-read'],
            'aws-exec-read'             => $config['aws-exec-read'],
            'bucket-owner-read'         => $config['bucket-owner-read'],
            'bucket-owner-full-control' => $config['bucket-owner-full-control'],
        ));

        $T->parse('output', 'blockheader-message');
        $content .= $T->finish($T->get_var('output'));
    }

    $T->parse('output', 'topmenu');
    $content .= $T->finish($T->get_var('output'));

    $T->parse('output', 'config');
    $content .= $T->finish($T->get_var('output'));

    $display = COM_createHTMLDocument($content);
    COM_output($display);
}

/*
* AWSS3_admindefault
*/
function AWSS3_admindefault($msg) {
    global $_CONF;

    $content = "";

    // JS required by the delete feature
    $content .= '<script type="text/javascript">/* quick Javascript confirmation function */';
    $content .= 'function confirm_action(msg,url){if(confirm(msg)){location.href=url;}}';
    $content .= '</script>';    

    $list = AWSS3_getlist(False);
    if($list['html'] == ""){
        $notfound = "- 見つかりませんでした -";
    }
    $msg .= $list['error'];

    $T = new Template($_CONF['path'] . 'plugins/awss3/templates');
    
    if($msg == "") {
        $T->set_file(array (
            'topmenu'     => 'topmenu.thtml',
            'table'       => 'table.thtml',
        ));

        $T->set_var(array(
            'lang_piname' => AWSS3_str('admin_label'),
            'icon_url'    => $_CONF['site_url'] . '/awss3/images/Storage_AmazonS3.png',
            'list'        => $list['html'],
            'notfound'    => $notfound,
        ));

    }
    else {
        $T->set_file(array (
            'blockheader-message' => 'blockheader-message.thtml',
            'topmenu'             => 'topmenu.thtml',
            'table'               => 'table.thtml',
        ));

        $T->set_var(array(
            'msg_title'   => 'AWS S3 Uploder Message',
            'msg_text'    => $msg,
            'lang_piname' => AWSS3_str('admin_label'),
            'icon_url'    => $_CONF['site_url'] . '/awss3/images/Storage_AmazonS3.png',
            'list'        => $list['html'],
            'notfound'    => $notfound,
        ));

        $T->parse('output', 'blockheader-message');
        $content .= $T->finish($T->get_var('output'));
    }

    $T->parse('output', 'topmenu');
    $content .= $T->finish($T->get_var('output'));

    $T->parse('output', 'blockheader');
    $content .= $T->finish($T->get_var('output'));
    
    $T->parse('output', 'awss3upload');
    $content .= $T->finish($T->get_var('output'));

    $T->parse('output', 'table');
    $content .= $T->finish($T->get_var('output'));

    $display = COM_createHTMLDocument($content);
    COM_output($display);
}

/*
* AWSS3_default
*/
function AWSS3_default($msg) {
    global $_CONF, $_USER;

    $content = "";
    // JS required by the delete feature
    $content .= '<script type="text/javascript">';
    $content .= 'function confirm_action(msg,url){if(confirm(msg)){location.href=url;}}';
    $content .= '</script>';    

    $list = AWSS3_getlist();

    if($list['html'] == ""){
        $notfound = "- 見つかりませんでした -";
    }
    $msg .= $list['error'];

    $T = new Template($_CONF['path'] . 'plugins/awss3/templates');

    if($msg == "") {
        $T->set_file(array (
            'blockheader' => 'blockheader.thtml',
            'awss3upload' => 'awss3upload.thtml',
            'table'       => 'table.thtml',
        ));

        $T->set_var(array(
            'lang_piname' => AWSS3_str('admin_label'),
            'icon_url'    => $_CONF['site_url'] . '/awss3/images/Storage_AmazonS3.png',
            'uname'       => $_USER['fullname'],
            'meta1'       => '投稿者',
            'meta2'       => 'タイトル',
            'list'        => $list['html'],
            'notfound'    => $notfound,
        ));

    }
    else {
        $T->set_file(array (
            'blockheader'         => 'blockheader.thtml',
            'blockheader-message' => 'blockheader-message.thtml',
            'awss3upload'         => 'awss3upload.thtml',
            'table'               => 'table.thtml',
        ));

        $T->set_var(array(
            'lang_piname' => AWSS3_str('admin_label'),
            'icon_url'    => $_CONF['site_url'] . '/awss3/images/Storage_AmazonS3.png',
            'msg_title'   => 'AWS S3 Uploder Message',
            'msg_text'    => $msg,
            'uname'       => $_USER['fullname'],
            'meta1'       => '投稿者',
            'meta2'       => 'タイトル',
            'list'        => $list['html'],
            'notfound'    => $notfound,
        ));

        $T->parse('output', 'blockheader-message');
        $content .= $T->finish($T->get_var('output'));
    }

    $T->parse('output', 'blockheader');
    $content .= $T->finish($T->get_var('output'));
    
    $T->parse('output', 'awss3upload');
    $content .= $T->finish($T->get_var('output'));

    $T->parse('output', 'table');
    $content .= $T->finish($T->get_var('output'));

    $display = COM_createHTMLDocument($content);
    COM_output($display);
}

/*
* AWSS3_download
*/
function AWSS3_download($key) {

    $config = AWSS3_getconfig();

    try {

        if($config == NULL) {
            $msg = "プラグインを使用する前に、まず初めにプラグインの管理者メニューから[設定画面]を開き、S3に接続するための情報を設定してください。";
        }
        else {
            $client = S3Client::factory([
                'credentials' => [
                'key'     => $config['iamkey'],
                'secret'  => $config['iamskey'],
                ],
                'region'  => $config['region'],
                'version' => 'latest',
            ]);

            $content = $client->getObject(array(
                'Bucket' => $config['bucket'],
                'Key'    => $key,
            ));

            $length = $content['ContentLength'];
            $content['Body']->rewind();
            $data = $content['Body']->read($length);
            $filename = end(explode('/', $key));

            header('Content-Type: application/octet-stream');
            $disposition = 'Content-Disposition: attachment; filename="' . $filename . '"';
            header($disposition);
            $contentlength = 'Content-Length: ' . $length;
            header($contentlength);
            echo $data;
        }
    } catch (S3Exception $exc) {
        AWSS3_default($exc->getMessage());
    }
}

/*
* AWSS3_upload
*/
function AWSS3_upload($files, $post) {
    global$_USER;

    $config = AWSS3_getconfig();

    try {
        if($config == NULL) {
            $msg = "プラグインを使用する前に、まず初めにプラグインの管理者メニューから[設定画面]を開き、S3に接続するための情報を設定してください。";
        }
        else {
            $client = S3Client::factory([
                'credentials' => [
                'key'     => $config['iamkey'],
                'secret'  => $config['iamskey'],
                ],
                'region'  => $config['region'],
                'version' => 'latest',
            ]);

            $result = $client->putObject([
                'Bucket'      => $config['bucket'],
                'Key'         => microtime()."_".basename($files['upfile']['name']),
                'SourceFile'  => $files['upfile']['tmp_name'],
                'ContentType' => $files['upfile']['type'],
                'ACL'         => $config['acl'],
                'Metadata'    => array(    
                    'user'    => $_USER['uid'],
                    'hdname'  => $post['hdname'],
                    'title'   => $post['title'],
                )
            ]);
            
            $msg = "登録しました。";
        }
        
        AWSS3_default($msg);
        
    } catch (S3Exception $exc) {
        AWSS3_default($exc->getMessage());
    }
}

/*
* AWSS3_delete
*/
function AWSS3_delete($key) {
    $config = AWSS3_getconfig();

    try {
        if($config == NULL)
        {
            $msg = "プラグインを使用する前に、まず初めにプラグインの管理者メニューから[設定画面]を開き、S3に接続するための情報を設定してください。";
        }
        else
        {
            $client = S3Client::factory([
                'credentials' => [
                'key'     => $config['iamkey'],
                'secret'  => $config['iamskey'],
                ],
                'region'  => $config['region'],
                'version' => 'latest',
            ]);
        
            $result = $client->deleteObject([
                'Bucket' => $config['bucket'],
                'Key'    => $key,
            ]);
            
            $msg = "削除しました。";
        }
        
        AWSS3_default($msg);

    } catch (S3Exception $exc) {
        AWSS3_default($exc->getMessage());
    }
}

/*
* AWSS3_getlist
*/
function AWSS3_getlist($filter = True) {
    global $_USER, $_CONF;

    $config = AWSS3_getconfig();

    try {
        if($config == NULL)
        {
            $ret['html'] = "";
            $ret['error'] = "プラグインを使用する前に、まず初めにプラグインの管理者メニューから[設定画面]を開き、S3に接続するための情報を設定してください。";
        }
        else
        {
            $client = S3Client::factory([
                'credentials' => [
                'key'     => $config['iamkey'],
                'secret'  => $config['iamskey'],
                ],
                'region'  => $config['region'],
                'version' => 'latest',
            ]);
    
            $result = $client->listObjects([
                'Bucket' => $config['bucket'],
            ]);

            if(isset($result['Contents'])){
                foreach ($result['Contents'] as $object) {
            
                    $content = $client->getObject(array(
                        'Bucket' => $config['bucket'],
                        'Key'    => $object['Key'],
                    ));

                    //$url = $client->getObjectUrl($config['bucket'], $object['Key']);
            
                    if($filter == True) {
                        if($content['Metadata']['user'] == $_USER['uid']) {
                        
                            $ret['html'] .= '<tr>'
                             .  '<td><a href="index.php?mode=delete&key=' . $object['Key'] . '" onclick="confirm_action(\'本当に削除してよいですか?\', \'index.php?mode=delete&key=' . $object['Key'] . '\')" id="' . $object['Key'] . '">'
                             .  '<img src="' . $_CONF['layout_url'] . '/images/deleteitem.png" style="vertical-align: middle;" alt="Delete" />'
                             .  '<script type="text/javascript">document.getElementById("' . $object['Key'] . '").href = "javascript:void(0);";</script></td>'
                             .  '<td>' . iconv_mime_decode($content['Metadata']['hdname'], 0, 'ISO-8859-1') . '</td>'
                             .  '<td>' . iconv_mime_decode($content['Metadata']['title'], 0, 'ISO-8859-1') . '</td>'
                             .  '<td><a href="index.php?mode=get&key=' . $object['Key'] . '">' . $object['Key'] . '</a></td>'
                             .  '<td>' . $object['LastModified'] . '</td>'
                             .  '</tr>'."\n";
                        }
                    }
                    else
                    {
                        $ret['html'] .= '<tr>'
                         .  '<td><a href="index.php?mode=delete&key=' . $object['Key'] . '" onclick="confirm_action(\'本当に削除してよいですか?\', \'index.php?mode=delete&key=' . $object['Key'] . '\')" id="' . $object['Key'] . '">'
                         .  '<img src="' . $_CONF['layout_url'] . '/images/deleteitem.png" style="vertical-align: middle;" alt="Delete" />'
                         .  '<script type="text/javascript">document.getElementById("' . $object['Key'] . '").href = "javascript:void(0);";</script></td>'
                         .  '<td>' . iconv_mime_decode($content['Metadata']['hdname'], 0, 'ISO-8859-1') . '</td>'
                         .  '<td>' . iconv_mime_decode($content['Metadata']['title'], 0, 'ISO-8859-1') . '</td>'
                         .  '<td><a href="index.php?mode=get&key=' . $object['Key'] . '">' . $object['Key'] . '</a></td>'
                         .  '<td>' . $object['LastModified'] . '</td>'
                         .  '</tr>'."\n";
                    }
                }
            }
        }

        return $ret;
        
    } catch (S3Exception $exc) {
        $ret['error'] = $exc->getMessage();
        return $ret;
    }
}

/*
* AWSS3_getconfig
*/
function AWSS3_getconfig() {
    global $_TABLES;
  
    $sql = "select iamkey,iamskey,region,bucket,acl FROM {$_TABLES['awss3']}";
    $result = DB_query($sql);
    if(DB_numrows($result) > 0) {
        $config = DB_fetchArray($result);
        $config['us-east-1'] = "";
        $config['us-east-2'] = "";
        $config['us-west-1'] = "";
        $config['us-west-2'] = "";
        $config['ca-central-1'] = "";
        $config['eu-central-1'] = "";
        $config['eu-west-1'] = "";
        $config['eu-west-2'] = "";
        $config['eu-west-3'] = "";
        $config['ap-northeast-1'] = "";
        $config['ap-northeast-2'] = "";
        $config['ap-northeast-3'] = "";
        $config['ap-southeast-1'] = "";
        $config['ap-southeast-2'] = "";
        $config['ap-south-1'] = "";
        $config['sa-east-1'] = "";
        $config[$config['region']] = "selected";

        $config['private'] = "";
        $config['public-read'] = "";
        $config['public-read-write'] = "";
        $config['authenticated-read'] = "";
        $config['aws-exec-read'] = "";
        $config['bucket-owner-read'] = "";
        $config['bucket-owner-full-control'] = "";
        $config[$config['acl']] = "selected";
    }
  
  return $config;
}

?>
