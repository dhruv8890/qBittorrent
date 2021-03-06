/*
 * Bittorrent Client using Qt and libtorrent.
 * Copyright (C) 2016  Vladimir Golovnev <glassez@yandex.ru>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * In addition, as a special exception, the copyright holders give permission to
 * link this program with the OpenSSL project's "OpenSSL" library (or with
 * modified versions of it that use the same license as the "OpenSSL" library),
 * and distribute the linked executables. You must obey the GNU General Public
 * License in all respects for all of the code used other than "OpenSSL".  If you
 * modify file(s), you may extend this exception to your version of the file(s),
 * but you are not obligated to do so. If you do not wish to do so, delete this
 * exception statement from your version.
 */

#include "cookiesdialog.h"

#include "base/settingsstorage.h"
#include "base/net/downloadmanager.h"
#include "guiiconprovider.h"
#include "cookiesmodel.h"
#include "ui_cookiesdialog.h"

#define SETTINGS_KEY(name) "CookiesDialog/" name
const QString KEY_GEOMETRY = SETTINGS_KEY("Geometry");
const QString KEY_COOKIESVIEWSTATE = SETTINGS_KEY("CookiesViewState");

CookiesDialog::CookiesDialog(QWidget *parent)
    : QDialog(parent)
    , m_ui(new Ui::CookiesDialog)
    , m_cookiesModel(new CookiesModel(Net::DownloadManager::instance()->allCookies(), this))
{
    m_ui->setupUi(this);

    setWindowIcon(GuiIconProvider::instance()->getIcon("preferences-web-browser-cookies"));
    m_ui->buttonAdd->setIcon(GuiIconProvider::instance()->getIcon("list-add"));
    m_ui->buttonDelete->setIcon(GuiIconProvider::instance()->getIcon("list-remove"));

    m_ui->treeView->setModel(m_cookiesModel);
    if (m_cookiesModel->rowCount() > 0)
        m_ui->treeView->selectionModel()->setCurrentIndex(
                    m_cookiesModel->index(0, 0),
                    QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);

    restoreGeometry(SettingsStorage::instance()->loadValue(KEY_GEOMETRY).toByteArray());
    m_ui->treeView->header()->restoreState(
                SettingsStorage::instance()->loadValue(KEY_COOKIESVIEWSTATE).toByteArray());
}

CookiesDialog::~CookiesDialog()
{
    SettingsStorage::instance()->storeValue(KEY_GEOMETRY, saveGeometry());
    SettingsStorage::instance()->storeValue(
                KEY_COOKIESVIEWSTATE, m_ui->treeView->header()->saveState());
    delete m_ui;
}

void CookiesDialog::accept()
{
    Net::DownloadManager::instance()->setAllCookies(m_cookiesModel->cookies());
    QDialog::accept();
}

void CookiesDialog::onButtonAddClicked()
{
    int row = m_ui->treeView->selectionModel()->currentIndex().row() + 1;

    m_cookiesModel->insertRow(row);
    m_ui->treeView->selectionModel()->setCurrentIndex(
                m_cookiesModel->index(row, 0), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
}

void CookiesDialog::onButtonDeleteClicked()
{
    m_cookiesModel->removeRow(m_ui->treeView->selectionModel()->currentIndex().row());
}
