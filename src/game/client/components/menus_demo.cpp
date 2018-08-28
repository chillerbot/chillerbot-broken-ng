/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */

#include "menus.h"

CMenus::CListboxItem CMenus::UiDoListboxNextRow()
{
	CListboxItem Item = {0};
	return Item;
}

CMenus::CListboxItem CMenus::UiDoListboxNextItem(const void *pId, bool Selected, bool KeyEvents)
{
	CListboxItem Item = UiDoListboxNextRow();
	return Item;
}

int CMenus::UiDoListboxEnd(float *pScrollValue, bool *pItemActivated)
{
	return 0;
}

int CMenus::DemolistFetchCallback(const char *pName, time_t Date, int IsDir, int StorageType, void *pUser)
{
	return 0;
}

void CMenus::DemolistPopulate()
{

}

void CMenus::DemolistOnUpdate(bool Reset)
{

}
