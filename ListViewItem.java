package caucse.myapplication;

import android.graphics.drawable.Drawable;

/**
 * Created by caucse on 2018-04-10.
 */

public class ListViewItem {
    private Drawable iconDrawable ;
    private String titleStr ;
    private String descStr ;

    public ListViewItem(Drawable iconDrawable, String titleStr, String descStr) {
        this.iconDrawable = iconDrawable;
        this.titleStr = titleStr;
        this.descStr = descStr;
    }

    public void setIcon(Drawable icon) {
        iconDrawable = icon ;
    }

    public void setTitle(String title) {

        titleStr = title ;
    }
    public void setDesc(String desc) {
        descStr = desc ;
    }

    public Drawable getIcon() {
        return this.iconDrawable ;
    }
    public String getTitle() {
        return this.titleStr ;
    }
    public String getDesc() {
        return this.descStr ;
    }
}