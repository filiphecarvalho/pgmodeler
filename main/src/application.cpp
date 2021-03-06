/*
# PostgreSQL Database Modeler (pgModeler)
#
# Copyright 2006-2015 - Raphael Araújo e Silva <raphael@pgmodeler.com.br>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation version 3.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# The complete text of GPLv3 is at LICENSE file on source code root directory.
# Also, you can get the complete GNU General Public License at <http://www.gnu.org/licenses/>
*/
#include "application.h"

Application::Application(int &argc, char **argv) : QApplication(argc,argv)
{
  QTranslator *main_translator=nullptr, *plugin_translator=nullptr;
  QFile ui_style(GlobalAttributes::CONFIGURATIONS_DIR +
                 GlobalAttributes::DIR_SEPARATOR +
                 GlobalAttributes::UI_STYLE_CONF +
                 GlobalAttributes::CONFIGURATION_EXT);
	QString plugin_name, plug_lang_dir, plug_lang_file;
	QStringList dir_list;

  //Changing the current working dir to the executable's directory in
  QDir::setCurrent(this->applicationDirPath());

  //Adding paths which executable will find plugins and it's dependecies
  this->addLibraryPath(this->applicationDirPath());
  this->addLibraryPath(GlobalAttributes::PLUGINS_DIR);

  //Tries to load the main ui translation according to the system's locale
  main_translator=new QTranslator;
  main_translator->load(QLocale::system().name(), GlobalAttributes::LANGUAGES_DIR);
  this->installTranslator(main_translator);

  //Trying to load plugins translations
  dir_list=QDir(GlobalAttributes::PLUGINS_DIR +
                GlobalAttributes::DIR_SEPARATOR,
                "*", QDir::Name, QDir::AllDirs | QDir::NoDotAndDotDot).entryList();

  while(!dir_list.isEmpty())
  {
    plugin_name=dir_list.front();
    dir_list.pop_front();

    //Configure the path to "lang" subdir at current plugin directory
    plug_lang_dir=GlobalAttributes::PLUGINS_DIR +
               GlobalAttributes::DIR_SEPARATOR + plugin_name +
               GlobalAttributes::DIR_SEPARATOR + "lang" +
               GlobalAttributes::DIR_SEPARATOR;

    plug_lang_file=plugin_name + "." + QLocale::system().name();

    //Check if the .qm file exists for the current plugin. If so create and install a translator
    if(QFileInfo(plug_lang_dir + plug_lang_file + ".qm").exists())
    {
      plugin_translator=new QTranslator;
      plugin_translator->load(plug_lang_file, plug_lang_dir);
      this->installTranslator(plugin_translator);
    }
  }

	//Loading app style sheet
	ui_style.open(QFile::ReadOnly);

	//Raises an error if ui style is not found
	if(!ui_style.isOpen())
	{
		Messagebox msg;
		msg.show(Exception(Exception::getErrorMessage(ERR_FILE_DIR_NOT_ACCESSED).arg(ui_style.fileName()),
											 ERR_FILE_DIR_NOT_ACCESSED,__PRETTY_FUNCTION__,__FILE__,__LINE__));
	}
	else
		this->setStyleSheet(ui_style.readAll());
}

bool Application::notify(QObject *receiver, QEvent *event)
{
  try
  {
    return(QApplication::notify(receiver,event));
  }
  catch(Exception &e)
  {
		Messagebox msg_box;
    msg_box.show(e);
    return(false);
  }
  catch(...)
  {
		Messagebox msg_box;
    msg_box.show(trUtf8("Unknown exception caught!"), Messagebox::ERROR_ICON);
    return(false);
  }
}
