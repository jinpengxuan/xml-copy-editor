<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
  <xsl:template match="LearningOutcomes">
    <h1>Learning outcomes</h1>
    <xsl:apply-templates select="Paragraph"/>
    <ul>
      <xsl:apply-templates select="LearningOutcome"/>
    </ul>
  </xsl:template>
  <xsl:template match="LearningOutcome">
    <li>
      <xsl:call-template name="apply-templates-lang"/>
    </li>
  </xsl:template>
  <xsl:template match="Glossary">
    <h1>Glossary</h1>
    <xsl:call-template name="apply-templates-lang"/>
  </xsl:template>
  <xsl:template match="GlossaryItem">
    <xsl:call-template name="apply-templates-lang"/>
  </xsl:template>
  <xsl:template match="GlossaryItem/Term"><p><em><xsl:call-template name="apply-templates-lang"
        /></em></p>  </xsl:template>
  <xsl:template match="GlossaryItem/Definition">
    <p>
      <xsl:call-template name="apply-templates-lang"/>
    </p>
  </xsl:template>
  <xsl:template match="CourseTeam">
    <xsl:if test="count(Heading) = 0">
      <h1>Course team</h1>
    </xsl:if>
    <xsl:call-template name="apply-templates-lang"/>
  </xsl:template>
  <xsl:template match="CourseTeam/Heading">
    <h1>
      <xsl:call-template name="apply-templates-lang"/>
    </h1>
  </xsl:template>
  <xsl:template match="CourseTeam/SubHeading">
    <h2>
      <xsl:call-template name="apply-templates-lang"/>
    </h2>
  </xsl:template>
  <xsl:template match="CourseTeam/SubSubHeading">
    <h3>
      <xsl:call-template name="apply-templates-lang"/>
    </h3>
  </xsl:template>
  <xsl:template match="Activity">
    <xsl:if test="count(Heading) = 0">
      <p>
        <span class="optional-prodnote">
          <strong>Activity</strong>
        </span>
      </p>
    </xsl:if>
    <xsl:call-template name="apply-templates-lang"/>
    <p>
      <span class="optional-prodnote">End of activity</span>
    </p>
  </xsl:template>
  <xsl:template match="SAQ">
    <xsl:if test="count(Heading) = 0">
      <p>
        <span class="optional-prodnote">
          <strong>SAQ</strong>
        </span>
      </p>
    </xsl:if>
    <xsl:call-template name="apply-templates-lang"/>
    <p>
      <span class="optional-prodnote">End of SAQ</span>
    </p>
  </xsl:template>
  <xsl:template match="Exercise">
    <xsl:if test="count(Heading) = 0">
      <p>
        <span class="optional-prodnote">
          <strong>Exercise</strong>
        </span>
      </p>
    </xsl:if>
    <xsl:call-template name="apply-templates-lang"/>
    <p>
      <span class="optional-prodnote">End of exercise</span>
    </p>
  </xsl:template>
  <xsl:template match="ITQ">
    <xsl:if test="count(Heading) = 0">
      <p>
        <span class="optional-prodnote">
          <strong>ITQ</strong>
        </span>
      </p>
    </xsl:if>
    <xsl:call-template name="apply-templates-lang"/>
    <p>
      <span class="optional-prodnote">End of ITQ</span>
    </p>
  </xsl:template>
  <xsl:template match="Figure">
    <p>
      <span class="optional-prodnote">Figure</span>
    </p>
    <xsl:choose>
      <xsl:when test="string-length(./Image/@src) &gt; 0">
        <p>
          <img>
            <xsl:attribute name="src">
              <xsl:value-of select="./Image/@src"/>
            </xsl:attribute>
            <xsl:attribute name="alt">
              <xsl:choose>
                <xsl:when
                  test="count(./Description/*) = 0 and string-length(./Description) &lt; 1024">
                  <xsl:value-of select="./Description"/>
                </xsl:when>
                <xsl:when test="string-length(./Image/@alt) &gt; 0">
                  <xsl:value-of select="./Image/@alt"/>
                </xsl:when>
                <xsl:when test="string-length(Alternative) &gt; 0">
                  <xsl:value-of select="Alternative"/>
                </xsl:when>
                <xsl:when test="count(./Description/*) &gt; 0">
                  <xsl:text>figure description follows</xsl:text>
                </xsl:when>
                <xsl:otherwise>
                  <xsl:text>no alternative text</xsl:text>
                </xsl:otherwise>
              </xsl:choose>
            </xsl:attribute>
          </img>
        </p>
      </xsl:when>
      <xsl:otherwise>
        <p>
          <xsl:apply-templates select="./Image/text() | ./Image/* "/>
        </p>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:apply-templates select="Caption"/>
    <xsl:if test="count(./Description/*) &gt; 0 or string-length(./Description) &gt; 1024">
      <xsl:apply-templates select="Description"/>
    </xsl:if>
    <xsl:apply-templates select="SourceReference"/>
    <p>
      <span class="optional-prodnote">End of figure</span>
    </p>
  </xsl:template>
  <xsl:template match="Equation">
    <p>
      <span class="optional-prodnote">Equation</span>
    </p>
    <xsl:choose>
      <xsl:when test="string-length(./Image/@src) &gt; 0">
        <p>
          <img>
            <xsl:attribute name="src">
              <xsl:value-of select="./Image/@src"/>
            </xsl:attribute>
            <xsl:attribute name="alt">
              <xsl:choose>
                <xsl:when
                  test="count(./Description/*) = 0 and string-length(./Description) &lt; 1024">
                  <xsl:value-of select="./Description"/>
                </xsl:when>
                <xsl:when test="string-length(./Image/@alt) &gt; 0">
                  <xsl:value-of select="./Image/@alt"/>
                </xsl:when>
                <xsl:when test="string-length(Alternative) &gt; 0">
                  <xsl:value-of select="Alternative"/>
                </xsl:when>
                <xsl:when test="count(./Description/*) &gt; 0">
                  <xsl:text>equation description follows</xsl:text>
                </xsl:when>
                <xsl:otherwise>
                  <xsl:text>no alternative text</xsl:text>
                </xsl:otherwise>
              </xsl:choose>
            </xsl:attribute>
          </img>
        </p>
      </xsl:when>
      <xsl:otherwise>
        <p>
          <xsl:apply-templates select="./Image/text() | ./Image/* "/>
        </p>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:apply-templates select="Label"/>
    <xsl:apply-templates select="TeX"/>
    <xsl:apply-templates select="MathML"/>
    <xsl:apply-templates select="Caption"/>
    <xsl:if test="count(./Description/*) &gt; 0 or string-length(./Description) &gt; 1024">
      <xsl:apply-templates select="Description"/>
    </xsl:if>
    <xsl:apply-templates select="SourceReference"/>
    <span class="optional-prodnote">End of equation</span>
  </xsl:template>
  <xsl:template match="Equation/TeX">
    <p>
      <span class="optional-prodnote">TeX source</span>
    </p>
    <p>
      <xsl:call-template name="apply-templates-lang"/>
    </p>
  </xsl:template>
  <xsl:template match="Equation/MathML">
    <!-- omit -->
  </xsl:template>
  <xsl:template match="Equation/Image">
    <p>
      <img>
        <xsl:attribute name="src">
          <xsl:value-of select="./Image/@src"/>
        </xsl:attribute>
        <xsl:attribute name="alt">
          <xsl:value-of select="./Image/@alt"/>
        </xsl:attribute>
      </img>
    </p>
  </xsl:template>
  <xsl:template match="References">
    <h1>References</h1>
    <xsl:call-template name="apply-templates-lang"/>
  </xsl:template>
  <xsl:template match="Reference">
    <p>
      <xsl:call-template name="apply-templates-lang"/>
    </p>
  </xsl:template>
  <xsl:template match="Box">
    <p>
      <span class="optional-prodnote">Box</span>
    </p>
    <xsl:if test="count(Heading) = 0">
      <p>
        <span class="optional-prodnote">
          <strong>Box</strong>
        </span>
      </p>
    </xsl:if>
    <xsl:call-template name="apply-templates-lang"/>
    <p>
      <span class="optional-prodnote">End of box</span>
    </p>
  </xsl:template>
  <xsl:template match="CaseStudy">
    <xsl:if test="count(Heading) = 0">
      <p>
        <span class="optional-prodnote">
          <strong>Case study</strong>
        </span>
      </p>
    </xsl:if>
    <xsl:call-template name="apply-templates-lang"/>
    <p>
      <span class="optional-prodnote">End of case study</span>
    </p>
  </xsl:template>
  <xsl:template match="Example">
    <xsl:if test="count(Heading) = 0">
      <p>
        <span class="optional-prodnote">
          <strong>Example</strong>
        </span>
      </p>
    </xsl:if>
    <xsl:call-template name="apply-templates-lang"/>
    <p>
      <span class="optional-prodnote">End of example</span>
    </p>
  </xsl:template>
  <xsl:template match="Extract">
    <xsl:if test="count(Heading) = 0">
      <p>
        <span class="optional-prodnote">
          <strong>Extract</strong>
        </span>
      </p>
    </xsl:if>
    <xsl:call-template name="apply-templates-lang"/>
    <p>
      <span class="optional-prodnote">End of extract</span>
    </p>
  </xsl:template>
  <xsl:template match="Verse">
    <xsl:if test="count(Heading) = 0">
      <p>
        <span class="optional-prodnote">
          <strong>Verse</strong>
        </span>
      </p>
    </xsl:if>
    <xsl:call-template name="apply-templates-lang"/>
    <p>
      <span class="optional-prodnote">End of verse</span>
    </p>
  </xsl:template>
  <xsl:template match="Dialogue">
    <xsl:if test="count(Heading) = 0">
      <p>
        <span class="optional-prodnote">
          <strong>Dialogue</strong>
        </span>
      </p>
    </xsl:if>
    <xsl:call-template name="apply-templates-lang"/>
    <p>
      <span class="optional-prodnote">End of dialogue</span>
    </p>
  </xsl:template>
  <xsl:template match="Dialogue/Speaker">
    <p>
      <xsl:call-template name="apply-templates-lang"/>
    </p>
  </xsl:template>
  <xsl:template match="Dialogue/Remark">
    <p>
      <xsl:call-template name="apply-templates-lang"/>
    </p>
  </xsl:template>
  <xsl:template match="Reading">
    <xsl:if test="count(Heading) = 0">
      <p>
        <span class="optional-prodnote">
          <strong>Reading</strong>
        </span>
      </p>
    </xsl:if>
    <xsl:call-template name="apply-templates-lang"/>
    <p>
      <span class="optional-prodnote">End of reading</span>
    </p>
  </xsl:template>
  <xsl:template match="ProgramListing">
    <p>
      <span class="optional-prodnote">
        <strong>Program listing</strong>
      </span>
    </p>
    <xsl:call-template name="apply-templates-lang"/>
    <p>
      <span class="optional-prodnote">End of program listing</span>
    </p>
  </xsl:template>
  <xsl:template match="ComputerDisplay">
    <p>
      <span class="optional-prodnote">
        <strong>Computer display</strong>
      </span>
    </p>
    <xsl:call-template name="apply-templates-lang"/>
    <p>
      <span class="optional-prodnote">End of computer display</span>
    </p>
  </xsl:template>
  <xsl:template match="ProgramListing/Paragraph | ComputerDisplay/Paragraph">
    <pre>
      <xsl:call-template name="apply-templates-lang"/>
    </pre>
  </xsl:template>
  <xsl:template match="ProgramListing/Paragraph/br | ComputerDisplay/Paragraph/br">
    <br/>
  </xsl:template>
  <xsl:template match="StudyNote">
    <xsl:if test="count(Heading) = 0">
      <p>
        <span class="optional-prodnote">Study note</span>
      </p>
    </xsl:if>
    <xsl:call-template name="apply-templates-lang"/>
    <p>
      <span class="optional-prodnote">End of study note</span>
    </p>
  </xsl:template>
  <xsl:template match="Transcript">
    <xsl:if test="count(Heading) = 0">
      <p>
        <span class="optional-prodnote">
          <strong>Transcript</strong>
        </span>
      </p>
    </xsl:if>
    <xsl:call-template name="apply-templates-lang"/>
    <p>
      <span class="optional-prodnote">End of transcript</span>
    </p>
  </xsl:template>
  <xsl:template match="KeyPoints">
    <p>
      <span class="optional-prodnote">Key points</span>
    </p>
    <xsl:apply-templates select="Paragraph"/>
    <ul>
      <xsl:apply-templates select="KeyPoint"/>
    </ul>
    <p>
      <span class="optional-prodnote">End of key points</span>
    </p>
  </xsl:template>
  <xsl:template match="KeyPoints/KeyPoint">
    <li>
      <xsl:call-template name="apply-templates-lang"/>
    </li>
  </xsl:template>
  <xsl:template match="Summary">
    <xsl:if test="count(Heading) = 0">
      <p>
        <span class="optional-prodnote">
          <strong>Summary</strong>
        </span>
      </p>
    </xsl:if>
    <xsl:call-template name="apply-templates-lang"/>
    <p>
      <span class="optional-prodnote">End of summary</span>
    </p>
  </xsl:template>
  <xsl:template match="SideNote">
    <hr/>
    <p><span class="optional-prodnote">Side note</span></p>
    <xsl:apply-templates/>
    <p><span class="optional-prodnote">End of side note</span></p>
    <hr/>
  </xsl:template>
  <xsl:template match="footnote">
    <sup>
      <xsl:value-of select="count(preceding::footnote) + 1"/>
    </sup>
  </xsl:template>

</xsl:stylesheet>
