/* Copyright (C) 2013, Frank Mueller <frmimue@gmail.com>

   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   - Redistributions of source code must retain the above copyright notice,
	 this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright notice,
	 this list of conditions and the following disclaimer in the documentation
	 and/or other materials provided with the distribution.
   - Neither the name of the Mumble Developers nor the names of its
	 contributors may be used to endorse or promote products derived from this
	 software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef MUMBLE_MUMBLE_LOGTABS_H
#define MUMBLE_MUMBLE_LOGTABS_H

#include <QTabWidget>

#include "ui_Log.h"
#include "CustomElements.h"

class ClientUser;

///Wrapping LogTextBrowser, to provide additional functionality for the tabs
class LogTab : public LogTextBrowser{
	Q_OBJECT
	///Access to LogTab members should only be possible for LogTabWidget
	friend class LogTabWidget;
	
public slots:
	void onHighlighted(const QUrl&);
	
private:
	///Initialize a special, user-defined LogTab
	LogTab(QString name, QString hash, QWidget *p = NULL);
	virtual ~LogTab();
	
	void addToTabWidget(QTabWidget *tabWidget);
	
	QString m_name;
	QString m_hash;
};

/// QTabWidget to manage LogTab instances.
/// Handles tab context menues, states and so on.
class LogTabWidget : public QTabWidget {
	Q_OBJECT
public:
	LogTabWidget(QWidget* parent = 0);
	virtual ~LogTabWidget();
	
	/// Set the visibility of the tabbar.
	void activateTabs(bool show);
	/// Returns the index of the fixed channel tab.
	int getChannelTab() const;
	
	/// Sets the user tab as current tab
	void openTab(ClientUser*);
	
	/// Returns the tab index for the given user's tab, -1 otherwise.
	int getUserTab(ClientUser *user) const;
	/// Returns the tab index for the given user's tab, creates one if it doesn't exist.
	int getOrCreateUserTab(ClientUser *user);
	
	/// Return the hash identifier for the tab at the given index.
	QString getHash(int index) const;
	/// Updates existing user tab from user object
	void updateTab(ClientUser* user);
	
	/// Adds a visible notification, indicating the related tab has been updated
	void markTabAsUpdated(int index);
	/// Adds a visible notification, indicating the related tab has been restricted
	void markTabAsRestricted(int index);
	/// Removes marks from given tab
	void unmarkTab(int newIndex);
	
	/// Sets the maximum block count for all tabs
	void handleDocumentsetMaximumBlockCount(int maxLogBlocks);
	/// Sets the default stylesheet for all tabs
	void handleDocumentSetDefaultStyleSheet(QString styleSheet);

public slots:
	void onCurrentChanged(int newIndex);
	void onTabMoved(int, int);
	void onTabCloseRequested(int index);
	void onTabBarCustomContextMenuRequested(const QPoint&);
	
signals:
	/// Emitted if any tab emits anchorClick
	void anchorClick(const QUrl& url);
	/// Emitted if any tab emits customContextMenuRequest
	void customContextMenuRequest(const QPoint& point);
	/// Emitted if any tab emits highlighted
	void highlighted(const QUrl& url);
	
private:
	int createTabFromUser(ClientUser* user);
	int createTab(QString name, QString hash);
	void updateHashMap();
	
	/// Mapping a users hash to a tab index
	QHash<QString, int> m_hashMap;
	int m_maxBlockCount;
};

#endif
