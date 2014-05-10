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

#include "LogTabs.h"
#include "ClientUser.h"
#include "ui_MainWindow.h"
#include "Log.h"

#include <QtCore/QtGlobal>
#include <QSignalMapper>

LogTab::LogTab(QString name, QString hash, QWidget *p) : LogTextBrowser(p) {
	m_hash = hash;
	m_name = name;
	
	setFrameStyle(QFrame::NoFrame);
	setOpenLinks(false);
	setContextMenuPolicy(Qt::CustomContextMenu);
	
	LogDocument* document = new LogDocument(this);
	
	document->setDefaultStyleSheet(qApp->styleSheet());
	
	setDocument(document);
	
	connect(this, SIGNAL(highlighted(QUrl)), this, SLOT(onHighlighted(QUrl)));
	
	setWhatsThis(tr("This shows all recent activity. Connecting to servers, errors and information messages all show up here.&lt;br /&gt;To configure exactly which messages show up here, use the &lt;b&gt;Settings&lt;/b&gt; command from the menu."));
}


LogTab::~LogTab() {
	// Nothing
}

void LogTab::addToTabWidget(QTabWidget *tabWidget) {
	tabWidget->addTab(this, m_name);
}

void LogTab::onHighlighted(const QUrl& url) {
	if (QString::fromLatin1("clientid") == url.scheme() || QString::fromLatin1("channelid") == url.scheme())
		return;
	
	if (! url.isValid())
		QToolTip::hideText();
	else
		QToolTip::showText(QCursor::pos(), url.toString(), this, QRect());
}

LogTabWidget::LogTabWidget(QWidget* parent) : QTabWidget(parent) {
	setTabPosition(QTabWidget::South);
	setMovable(true);
	setUsesScrollButtons(true);
	setElideMode(Qt::ElideRight);

	tabBar()->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(this, SIGNAL(currentChanged(int)), this, SLOT(onCurrentChanged(int)));
	connect(tabBar(), SIGNAL(tabMoved(int, int)), this, SLOT(onTabMoved(int, int)));
	connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(onTabCloseRequested(int)));
	connect(tabBar(), SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onTabBarCustomContextMenuRequested(QPoint)));

	const int channelTab = createTab(tr("channel"), QLatin1String("channel"));
	setTabText(channelTab, tr("Not connected"));
	setTabToolTip(channelTab, tr("Not connected"));
}

LogTabWidget::~LogTabWidget() {
	// Nothing
}

void LogTabWidget::activateTabs(bool show) {
	tabBar()->setVisible(show);
}

int LogTabWidget::getOrCreateUserTab(ClientUser *user){
	const int tab = getUserTab(user);
	if (tab == -1) {
		return createTabFromUser(user);
	}
	
	return tab;
}

int LogTabWidget::getUserTab(ClientUser *user) const {
	QHash<QString, int>::const_iterator iter = m_hashMap.find(user->qsHash);
	return iter != m_hashMap.constEnd() ? iter.value() : -1;
}

int LogTabWidget::createTabFromUser(ClientUser *user){
	return createTab(user->qsName, user->qsHash);
}

int LogTabWidget::createTab(QString name, QString hash) {
	LogTab* tab = new LogTab(name, hash, this);
	tab->addToTabWidget(this);
	tab->document()->setMaximumBlockCount(m_maxBlockCount);
	
	const int tabIdx = indexOf(tab);
	m_hashMap.insert(hash, tabIdx);
	setTabToolTip(tabIdx, tab->m_name);
	
	connect(tab, SIGNAL(anchorClicked(const QUrl&)), this, SIGNAL(anchorClick(const QUrl&)));
	connect(tab, SIGNAL(customContextMenuRequested(const QPoint&)), this, SIGNAL(customContextMenuRequest(const QPoint&)));
	connect(tab, SIGNAL(highlighted(QUrl)), this, SIGNAL(highlighted(QUrl)));
	
	return tabIdx;
}

void LogTabWidget::openTab(ClientUser* user) {
	setCurrentIndex(getOrCreateUserTab(user));
}

int LogTabWidget::getChannelTab() const {
	return m_hashMap.find(QLatin1String("channel")).value();
}

QString LogTabWidget::getHash(int index) const {
	return qobject_cast<LogTab *>(widget(index))->m_hash;
}

void LogTabWidget::updateHashMap() {
	m_hashMap.clear();
	for(int i = 0; i < count(); i++){
		m_hashMap.insert(qobject_cast<LogTab*>(widget(i))->m_hash, i);
	}
}

void LogTabWidget::markTabAsUpdated(int index) {
	if(currentIndex() != index) {
		const QColor& color = palette().color(QPalette::Disabled, QPalette::Link);
		tabBar()->setTabTextColor(index, color);
	}
}

void LogTabWidget::markTabAsRestricted(int index) {
	const QColor& color = palette().color(QPalette::Disabled, QPalette::WindowText);
	tabBar()->setTabTextColor(index, color);
}

void LogTabWidget::unmarkTab(int newIndex) {
	const QColor& color = palette().color(QPalette::WindowText);
	tabBar()->setTabTextColor(newIndex, color);	
}

void LogTabWidget::updateTab(ClientUser* user) {
	const int index = getUserTab(user);
	if(index == -1)
		return;
	
	LogTab* logTab = dynamic_cast<LogTab*>(widget(index));
	logTab->m_name = user->qsName;
	
	setTabText(index, logTab->m_name);
	setTabToolTip(index, logTab->m_name);
}

void LogTabWidget::handleDocumentsetMaximumBlockCount(int maxLogBlocks) {
	for(int i = 0; i < count(); i++){
		dynamic_cast<LogTab*>(widget(i))->document()->setMaximumBlockCount(maxLogBlocks);
	}
	m_maxBlockCount = maxLogBlocks;
}

void LogTabWidget::handleDocumentSetDefaultStyleSheet(QString styleSheet) {
	for(int i = 0; i < count(); i++){
		dynamic_cast<LogTab*>(widget(i))->document()->setDefaultStyleSheet(styleSheet);
	}
}

void LogTabWidget::onCurrentChanged(int newIndex) {
	unmarkTab(newIndex);
}

void LogTabWidget::onTabMoved(int, int) {
	updateHashMap();
}

void LogTabWidget::onTabCloseRequested(int index) {
	LogTab *tab = dynamic_cast<LogTab*>(widget(index));
	if (tab == NULL)
		return;
	
	m_hashMap.remove(tab->m_hash);
	removeTab(index);
	tab->deleteLater();

	updateHashMap();
}

void LogTabWidget::onTabBarCustomContextMenuRequested(const QPoint& point) {
	if (point.isNull())
		return;
	
	const int tab = tabBar()->tabAt(point);
	if (tab == getChannelTab()) {
		return;
	}
	
	QSignalMapper mapper;
	QMenu menu;
	
	QAction* action = menu.addAction(tr("Close"));
	mapper.setMapping(action, tab);
	connect(action, SIGNAL(triggered()), &mapper, SLOT(map()));
	connect(&mapper, SIGNAL(mapped(int)), this, SLOT(onTabCloseRequested(int)));
	connect(&mapper, SIGNAL(mapped(int)), this, SIGNAL(currentChanged(int)));
	menu.exec(QCursor::pos());
}
