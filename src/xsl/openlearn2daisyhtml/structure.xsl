<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
  <xsl:template match="Paragraph">
    <p>
      <xsl:call-template name="apply-templates-lang"/>
    </p>
  </xsl:template>
  <xsl:template match="Unit">
    <xsl:call-template name="apply-templates-lang"/>
  </xsl:template>
  <xsl:template match="UnitID"/>
  <xsl:template match="UnitTitle">
    <h1>
      <xsl:call-template name="apply-templates-lang"/>
    </h1>
  </xsl:template>
  <xsl:template match="Unit/ByLine">
    <p>
      <xsl:call-template name="apply-templates-lang"/>
    </p>
  </xsl:template>
  <xsl:template match="Unit/Introduction">
    <xsl:call-template name="apply-templates-lang"/>
  </xsl:template>
  <xsl:template match="Introduction/Title">
    <h1>
      <xsl:call-template name="apply-templates-lang"/>
    </h1>
  </xsl:template>
  <xsl:template match="Session">
    <xsl:call-template name="apply-templates-lang"/>
  </xsl:template>
  <xsl:template match="Session/Title">
    <h1>
      <xsl:call-template name="apply-templates-lang"/>
    </h1>
  </xsl:template>
  <xsl:template match="Session/Introduction">
    <xsl:call-template name="apply-templates-lang"/>
  </xsl:template>
  <xsl:template match="Session/Introduction/Title">
    <h2>
      <xsl:call-template name="apply-templates-lang"/>
    </h2>
  </xsl:template>
  <xsl:template match="Section">
    <xsl:call-template name="apply-templates-lang"/>
  </xsl:template>
  <xsl:template match="Section/Title">
    <h2>
      <xsl:call-template name="apply-templates-lang"/>
    </h2>
  </xsl:template>
  <xsl:template match="SubSection">
    <xsl:call-template name="apply-templates-lang"/>
  </xsl:template>
  <xsl:template match="SubSection/Title">
    <h3>
      <xsl:call-template name="apply-templates-lang"/>
    </h3>
  </xsl:template>
  <xsl:template match="SubSubSection">
    <xsl:call-template name="apply-templates-lang"/>
  </xsl:template>
  <xsl:template match="SubSubSection/Heading">
    <h4>
      <xsl:call-template name="apply-templates-lang"/>
    </h4>
  </xsl:template>
  <xsl:template match="Unit/BackMatter">
    <xsl:call-template name="apply-templates-lang"/>
  </xsl:template>
  <xsl:template match="Unit/Conclusion">
    <xsl:call-template name="apply-templates-lang"/>
  </xsl:template>
  <xsl:template match="Unit/Conclusion/Title">
    <h1>
      <xsl:call-template name="apply-templates-lang"/>
    </h1>
  </xsl:template>
  <xsl:template match="Acknowledgements">
    <xsl:if test="count(Heading) = '0'">
      <h1>Acknowledgements</h1>
    </xsl:if>
    <xsl:call-template name="apply-templates-lang"/>
  </xsl:template>
  <xsl:template match="Acknowledgements/Heading">
    <h1>
      <xsl:call-template name="apply-templates-lang"/>
    </h1>
  </xsl:template>
  <xsl:template match="Acknowledgements/SubHeading">
    <h2>
      <xsl:call-template name="apply-templates-lang"/>
    </h2>
  </xsl:template>
  <xsl:template match="Acknowledgements/SubSubHeading">
    <h3>
      <xsl:call-template name="apply-templates-lang"/>
    </h3>
  </xsl:template>
  <xsl:template match="Appendices">
    <xsl:call-template name="apply-templates-lang"/>
  </xsl:template>
  <xsl:template match="Appendix">
    <xsl:call-template name="apply-templates-lang"/>
  </xsl:template>
  <xsl:template match="Appendix/Heading">
    <h1>
      <xsl:call-template name="apply-templates-lang"/>
    </h1>
  </xsl:template>
  <xsl:template match="SourceReference">
    <p>
      <span class="optional-prodnote">
        <em>Source</em>
      </span>
    </p>
    <p>
      <xsl:call-template name="apply-templates-lang"/>
    </p>
  </xsl:template>
  <xsl:template match="InternalSection">
    <xsl:call-template name="apply-templates-lang"/>
  </xsl:template>
  <xsl:template match="MediaContent">
    <xsl:if test="@type = 'audio'">
      <p><span class="required-prodnote">[Audio content]</span></p>
      <p>
        <a><xsl:attribute name="href"><xsl:value-of select="@src"/>.mp3</xsl:attribute>MP3 file
      </a>
      </p>
    </xsl:if>
    <xsl:if test="@type = 'pdf'">
      <p><span class="required-prodnote">[PDF content]</span></p>
      <!--<p><a><xsl:attribute name="href"><xsl:value-of select="@src"/>.pdf</xsl:attribute></a></p>-->
    </xsl:if>
    <xsl:call-template name="apply-templates-lang"/>
    <p>
      <span class="optional-prodnote">End of <xsl:value-of select="@type"/> content</span>
    </p>
  </xsl:template>
  <xsl:template match="a">
    <a href="{@href}">
      <xsl:call-template name="apply-templates-lang"/>
    </a>
  </xsl:template>
  <xsl:template match="NumberedList | NumeredSubsidiaryList">
    <ol>
      <xsl:call-template name="apply-templates-lang"/>
    </ol>
  </xsl:template>
  <xsl:template match="BulletedList | UnNumberedList | BulletedSubsidiaryList | UnNumberedSubsidiaryList">
    <ul>
      <xsl:call-template name="apply-templates-lang"/>
    </ul>
  </xsl:template>
  <xsl:template match="ListItem | SubListItem">
    <li>
      <xsl:call-template name="apply-templates-lang"/>
    </li>
  </xsl:template>
  <xsl:template match="Caption">
    <p>
      <xsl:call-template name="apply-templates-lang"/>
    </p>
  </xsl:template>
  <xsl:template match="Alternative">
    <p>
      <xsl:call-template name="apply-templates-lang"/>
    </p>
  </xsl:template>
  <xsl:template match="Description">
    <p>
      <xsl:call-template name="apply-templates-lang"/>
    </p>
  </xsl:template>
  <xsl:template match="Label">
    <p>
      <xsl:call-template name="apply-templates-lang"/>
    </p>
  </xsl:template>
  <xsl:template match="Table">
    <p>
      <span class="optional-prodnote">Table</span>
    </p>
    <xsl:call-template name="apply-templates-lang"/>
    <p>
      <span class="optional-prodnote">End of table</span>
    </p>
  </xsl:template>
  <xsl:template match="TableHead">
    <p>
      <strong>
        <xsl:call-template name="apply-templates-lang"/>
      </strong>
    </p>
  </xsl:template>
  <xsl:template match="TableBody">
    <xsl:call-template name="apply-templates-lang"/>
  </xsl:template>
  <xsl:template match="TableFootnote">
    <p>
      <xsl:call-template name="apply-templates-lang"/>
    </p>
  </xsl:template>
  <xsl:template match="table"/>
  <xsl:template match="tbody">
    <table>
      <xsl:call-template name="apply-templates-lang"/>
    </table>
  </xsl:template>
  <xsl:template match="th">
    <th>
      <xsl:call-template name="apply-templates-lang"/>
    </th>
  </xsl:template>
  <xsl:template match="td">
    <td>
      <xsl:call-template name="apply-templates-lang"/>
    </td>
  </xsl:template>
  <xsl:template match="tr">
    <tr>
      <xsl:call-template name="apply-templates-lang"/>
    </tr>
  </xsl:template>
  <xsl:template match="Timing">
    <p>
      <xsl:call-template name="apply-templates-lang"/>
    </p>
  </xsl:template>
  <xsl:template match="Timing/Hours"><br/><xsl:call-template name="apply-templates-lang"/> hours<br/>
</xsl:template>
  <xsl:template match="Timing/Minutes"><br/><xsl:call-template name="apply-templates-lang"/> minutes<br/>
</xsl:template>
  <xsl:template match="Quote">
    <blockquote>
      <xsl:call-template name="apply-templates-lang"/>
    </blockquote>
  </xsl:template>
  <xsl:template match="Imprint"/>
</xsl:stylesheet>
