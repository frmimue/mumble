/* Copyright (C) 2005-2009, Thorvald Natvig <thorvald@natvig.com>

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

#ifndef _CERT_H
#define _CERT_H

class CertView : public QGroupBox {
	protected:
		QList<QSslCertificate> qlCert;
		QLabel *qlSubjectName, *qlSubjectEmail, *qlIssuerName;
	public:
		CertView(QWidget *p);
		void setCert(const QList<QSslCertificate> &cert);
};

#include "ui_Cert.h"


class CertWizard : public QWizard, public Ui::Certificates {
	private:
		Q_OBJECT
		Q_DISABLE_COPY(CertWizard)
	protected:
		QList<QSslCertificate> qscCurrent, qscNew;
		QSslKey qskCurrent, qskNew;

		QMap<QString, QHostInfo> qmDomains;
		bool bPendingDns;
		void generateNewCert();
		QByteArray exportCert();
	public:
		CertWizard(QWidget *p = NULL);
		int nextId() const;
		void initializePage(int);
		bool validateCurrentPage();
	public slots:
		void on_qleEmail_textChanged(const QString &);
		void on_qpbExportFile_clicked();
		void on_qleExportFile_textChanged(const QString &);
		void lookedUp(QHostInfo);
};

#endif