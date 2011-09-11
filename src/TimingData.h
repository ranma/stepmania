#ifndef TIMING_DATA_H
#define TIMING_DATA_H

#include "NoteTypes.h"
#include "TimingSegments.h"
#include "PrefsManager.h"
#include <float.h> // max float
struct lua_State;

/** @brief Compare a TimingData segment's properties with one another. */
#define COMPARE(x) if(this->x!=other.x) return false;


/* convenience functions to handle static casting */
template<class T> T* ToDerived( TimingSegment *t, TimingSegmentType tst )
{
	ASSERT( t->GetType() == tst ); // type checking
	return static_cast<T*>( t );
}

#define TimingSegmentToXWithName(Seg, SegName, SegType) \
	inline Seg* To##SegName( TimingSegment *t ) { return ToDerived<Seg>(t, SegType); }

#define TimingSegmentToX(Seg, SegType) \
	TimingSegmentToXWithName(Seg##Segment, Seg, SEGMENT_##SegType)

/* ToBPM(TimingSegment*), ToTimeSignature(TimingSegment*), etc. */
TimingSegmentToX( BPM, BPM );
TimingSegmentToX( Stop, STOP );
TimingSegmentToX( Delay, DELAY );
TimingSegmentToX( TimeSignature, TIME_SIG );
TimingSegmentToX( Warp, WARP );
TimingSegmentToX( Label, LABEL );
TimingSegmentToX( Tickcount, TICKCOUNT );
TimingSegmentToX( Combo, COMBO );
TimingSegmentToX( Speed, SPEED );
TimingSegmentToX( Scroll, SCROLL );
TimingSegmentToX( Fake, FAKE );

#undef TimingSegmentToXWithName
#undef TimingSegmentToX

/**
 * @brief Holds data for translating beats<->seconds.
 */
class TimingData
{
public:
	void AddSegment(TimingSegmentType tst, TimingSegment * seg);

	unsigned GetSegmentIndexAtRow(TimingSegmentType tst, int row) const;
	unsigned GetSegmentIndexAtBeat(TimingSegmentType tst, float beat) const
	{
		return GetSegmentIndexAtRow( tst, BeatToNoteRow(beat) );
	}

	float GetNextSegmentBeatAtRow(TimingSegmentType tst, int row) const;
	float GetNextSegmentBeatAtBeat(TimingSegmentType tst, float beat) const
	{
		return GetNextSegmentBeatAtRow( tst, BeatToNoteRow(beat) );
	}

	float GetPreviousSegmentBeatAtRow(TimingSegmentType tst, int row) const;
	float GetPreviousSegmentBeatAtBeat(TimingSegmentType tst, float beat) const
	{
		return GetPreviousSegmentBeatAtRow( tst, BeatToNoteRow(beat) );
	}

	bool empty() const;

	/**
	 * @brief Sets up initial timing data with a defined offset.
	 * @param fOffset the offset from the 0th beat. */
	TimingData(float fOffset = 0);
	~TimingData();

	TimingData CopyRange(int startRow, int endRow) const;
	/**
	 * @brief Gets the actual BPM of the song,
	 * while respecting a limit.
	 *
	 * The high limit is due to the implementation of mMods.
	 * @param fMinBPMOut the minimium specified BPM.
	 * @param fMaxBPMOut the maximum specified BPM.
	 * @param highest the highest allowed max BPM.
	 */
	void GetActualBPM( float &fMinBPMOut, float &fMaxBPMOut, float highest = FLT_MAX ) const;
	/**
	 * @brief Retrieve the BPM at the given row.
	 * @param iNoteRow the row in question.
	 * @return the BPM.
	 */
	float GetBPMAtRow( int iNoteRow ) const;
	/**
	 * @brief Retrieve the BPM at the given beat.
	 * @param fBeat the beat in question.
	 * @return the BPM.
	 */
	float GetBPMAtBeat( float fBeat ) const { return GetBPMAtRow( BeatToNoteRow(fBeat)); }
	/**
	 * @brief Set the row to have the new BPM.
	 * @param iNoteRow the row to have the new BPM.
	 * @param fBPM the BPM.
	 */
	void SetBPMAtRow( int iNoteRow, float fBPM );
	/**
	 * @brief Set the beat to have the new BPM.
	 * @param fBeat the beat to have the new BPM.
	 * @param fBPM the BPM.
	 */
	void SetBPMAtBeat( float fBeat, float fBPM ) { SetBPMAtRow( BeatToNoteRow(fBeat), fBPM ); }

	/**
	 * @brief Retrieve the TimingSegment at the specified row.
	 * @param iNoteRow the row that has a TimingSegment.
	 * @param tst the TimingSegmentType requested.
	 * @return the segment in question.
	 */
	TimingSegment* GetSegmentAtRow( int iNoteRow, TimingSegmentType tst );

	/**
	 * @brief Retrieve the TimingSegment at the given beat.
	 * @param fBeat the beat that has a TimingSegment.
	 * @param tst the TimingSegmentType requested.
	 * @return the segment in question.
	 */
	TimingSegment* GetSegmentAtBeat( float fBeat, TimingSegmentType tst )
	{
		return GetSegmentAtRow( BeatToNoteRow(fBeat), tst );
	}

	void SetTimingSegmentAtRow( TimingSegment *seg, int iNoteRow );


	/* XXX: convenience shortcuts. We should get rid of these later. */
	#define GetAndSetSegmentWithName(Seg, SegName, SegType) \
		Seg* Get##Seg##AtRow( int iNoteRow ) \
		{ \
			TimingSegment *t = GetSegmentAtRow( iNoteRow, SegType ); \
			return To##SegName( t ); \
		} \
		Seg* Get##Seg##AtBeat( float fBeat ) \
		{ \
			TimingSegment *t = GetSegmentAtBeat( fBeat, SegType ); \
			return To##SegName( t ); \
		} \
		void Set##SegName##AtRow( Seg &seg, int iNoteRow ) \
		{ \
			SetTimingSegmentAtRow( &seg, iNoteRow ); \
		}

	// (TimeSignature,TIME_SIG) -> (TimeSignatureSegment,SEGMENT_TIME_SIG)
	#define GetAndSetSegment(Seg, SegType ) \
		GetAndSetSegmentWithName( Seg##Segment, Seg, SEGMENT_##SegType )

	GetAndSetSegment( BPM, BPM );
	GetAndSetSegment( Stop, STOP );
	GetAndSetSegment( Delay, DELAY );
	GetAndSetSegment( Warp, WARP );
	GetAndSetSegment( Label, LABEL );
	GetAndSetSegment( Tickcount, TICKCOUNT );
	GetAndSetSegment( Combo, COMBO );
	GetAndSetSegment( Speed, SPEED );
	GetAndSetSegment( Scroll, SCROLL );
	GetAndSetSegment( Fake, FAKE );
	GetAndSetSegment( TimeSignature, TIME_SIG );

	#undef GetAndSetSegmentWithName
	#undef GetAndSetSegment


	/**
	 * @brief Retrieve the stop time at the given row.
	 * @param iNoteRow the row in question.
	 * @return the stop time.
	 */
	float GetStopAtRow( int iNoteRow ) const;
	/**
	 * @brief Retrieve the stop time at the given beat.
	 * @param fBeat the beat in question.
	 * @return the stop time.
	 */
	float GetStopAtBeat( float fBeat ) const { return GetStopAtRow( BeatToNoteRow(fBeat) ); }

	/**
	 * @brief Set the row to have the new stop time.
	 * @param iNoteRow the row to have the new stop time.
	 * @param fSeconds the new stop time.
	 */
	void SetStopAtRow( int iNoteRow, float fSeconds );
	/**
	 * @brief Set the beat to have the new stop time.
	 * @param fBeat to have the new stop time.
	 * @param fSeconds the new stop time.
	 */
	void SetStopAtBeat( float fBeat, float fSeconds ) { SetStopAtRow( BeatToNoteRow(fBeat), fSeconds); }

	/**
	 * @brief Retrieve the delay time at the given row.
	 * @param iNoteRow the row in question.
	 * @return the delay time.
	 */
	float GetDelayAtRow( int iNoteRow ) const;
	/**
	 * @brief Retrieve the delay time at the given beat.
	 * @param fBeat the beat in question.
	 * @return the delay time.
	 */
	float GetDelayAtBeat( float fBeat ) const { return GetDelayAtRow( BeatToNoteRow(fBeat) ); }

	/**
	 * @brief Set the row to have the new delay time.
	 *
	 * This function was added specifically for sm-ssc.
	 * @param iNoteRow the row to have the new delay time.
	 * @param fSeconds the new delay time.
	 */
	void SetDelayAtRow( int iNoteRow, float fSeconds );
	/**
	 * @brief Set the beat to have the new delay time.
	 *
	 * This function was added specifically for sm-ssc.
	 * @param fBeat the beat to have the new delay time.
	 * @param fSeconds the new delay time.
	 */
	void SetDelayAtBeat( float fBeat, float fSeconds ) { SetDelayAtRow( BeatToNoteRow(fBeat), fSeconds); }

	/**
	 * @brief Retrieve the Time Signature's numerator at the given row.
	 * @param iNoteRow the row in question.
	 * @return the numerator.
	 */
	int GetTimeSignatureNumeratorAtRow( int iNoteRow );
	/**
	 * @brief Retrieve the Time Signature's numerator at the given beat.
	 * @param fBeat the beat in question.
	 * @return the numerator.
	 */
	int GetTimeSignatureNumeratorAtBeat( float fBeat ) { return GetTimeSignatureNumeratorAtRow( BeatToNoteRow(fBeat) ); }
	/**
	 * @brief Retrieve the Time Signature's denominator at the given row.
	 * @param iNoteRow the row in question.
	 * @return the denominator.
	 */
	int GetTimeSignatureDenominatorAtRow( int iNoteRow );
 	/**
	 * @brief Retrieve the Time Signature's denominator at the given beat.
	 * @param fBeat the beat in question.
	 * @return the denominator.
	 */
	int GetTimeSignatureDenominatorAtBeat( float fBeat ) { return GetTimeSignatureDenominatorAtRow( BeatToNoteRow(fBeat) ); }
	/**
	 * @brief Set the row to have the new Time Signature.
	 * @param iNoteRow the row to have the new Time Signature.
	 * @param iNumerator the numerator.
	 * @param iDenominator the denominator.
	 */
	void SetTimeSignatureAtRow( int iNoteRow, int iNumerator, int iDenominator );
	/**
	 * @brief Set the beat to have the new Time Signature.
	 * @param fBeat the beat to have the new Time Signature.
	 * @param iNumerator the numerator.
	 * @param iDenominator the denominator.
	 */
	void SetTimeSignatureAtBeat( float fBeat, int iNumerator, int iDenominator ) { SetTimeSignatureAtRow( BeatToNoteRow(fBeat), iNumerator, iDenominator ); }
	/**
	 * @brief Set the row to have the new Time Signature numerator.
	 * @param iNoteRow the row to have the new Time Signature numerator.
	 * @param iNumerator the numerator.
	 */
	void SetTimeSignatureNumeratorAtRow( int iNoteRow, int iNumerator );
	/**
	 * @brief Set the beat to have the new Time Signature numerator.
	 * @param fBeat the beat to have the new Time Signature numerator.
	 * @param iNumerator the numerator.
	 */
	void SetTimeSignatureNumeratorAtBeat( float fBeat, int iNumerator ) { SetTimeSignatureNumeratorAtRow( BeatToNoteRow(fBeat), iNumerator); }
	/**
	 * @brief Set the row to have the new Time Signature denominator.
	 * @param iNoteRow the row to have the new Time Signature denominator.
	 * @param iDenominator the denominator.
	 */
	void SetTimeSignatureDenominatorAtRow( int iNoteRow, int iDenominator );
	/**
	 * @brief Set the beat to have the new Time Signature denominator.
	 * @param fBeat the beat to have the new Time Signature denominator.
	 * @param iDenominator the denominator.
	 */
	void SetTimeSignatureDenominatorAtBeat( float fBeat, int iDenominator ) { SetTimeSignatureDenominatorAtRow( BeatToNoteRow(fBeat), iDenominator); }

	/**
	 * @brief Determine the beat to warp to.
	 * @param iRow The row you start on.
	 * @return the beat you warp to.
	 */
	float GetWarpAtRow( int iRow ) const;
	/**
	 * @brief Determine the beat to warp to.
	 * @param fBeat The beat you start on.
	 * @return the beat you warp to.
	 */
	float GetWarpAtBeat( float fBeat ) const { return GetWarpAtRow( BeatToNoteRow( fBeat ) ); }
	/**
	 * @brief Set the beat to warp to given a starting row.
	 * @param iRow The row to start on.
	 * @param fNew The destination beat.
	 */
	void SetWarpAtRow( int iRow, float fNew );
	/**
	 * @brief Set the beat to warp to given a starting beat.
	 * @param fBeat The beat to start on.
	 * @param fNew The destination beat.
	 */
	void SetWarpAtBeat( float fBeat, float fNew ) { SetWarpAtRow( BeatToNoteRow( fBeat ), fNew ); }

	/**
	 * @brief Checks if the row is inside a warp.
	 * @param iRow the row to focus on.
	 * @return true if the row is inside a warp, false otherwise.
	 */
	bool IsWarpAtRow( int iRow ) const;
	/**
	 * @brief Checks if the beat is inside a warp.
	 * @param fBeat the beat to focus on.
	 * @return true if the row is inside a warp, false otherwise.
	 */
	bool IsWarpAtBeat( float fBeat ) const { return IsWarpAtRow( BeatToNoteRow( fBeat ) ); }
	
	/**
	 * @brief Retrieve the Tickcount at the given row.
	 * @param iNoteRow the row in question.
	 * @return the Tickcount.
	 */
	int GetTickcountAtRow( int iNoteRow ) const;
	/**
	 * @brief Retrieve the Tickcount at the given beat.
	 * @param fBeat the beat in question.
	 * @return the Tickcount.
	 */
	int GetTickcountAtBeat( float fBeat ) const { return GetTickcountAtRow( BeatToNoteRow(fBeat) ); }
	/**
	 * @brief Set the row to have the new tickcount.
	 * @param iNoteRow the row to have the new tickcount.
	 * @param iTicks the tickcount.
	 */
	void SetTickcountAtRow( int iNoteRow, int iTicks );
	/**
	 * @brief Set the beat to have the new tickcount.
	 * @param fBeat the beat to have the new tickcount.
	 * @param iTicks the tickcount.
	 */
	void SetTickcountAtBeat( float fBeat, int iTicks ) { SetTickcountAtRow( BeatToNoteRow( fBeat ), iTicks ); }

	/**
	 * @brief Retrieve the Combo at the given row.
	 * @param iNoteRow the row in question.
	 * @return the Combo.
	 */
	int GetComboAtRow( int iNoteRow ) const;
	/**
	 * @brief Retrieve the Combo at the given beat.
	 * @param fBeat the beat in question.
	 * @return the Combo.
	 */
	int GetComboAtBeat( float fBeat ) const { return GetComboAtRow( BeatToNoteRow(fBeat) ); }
	/**
	 * @brief Retrieve the Miss Combo at the given row.
	 * @param iNoteRow the row in question.
	 * @return the Miss Combo.
	 */
	int GetMissComboAtRow( int iNoteRow ) const;
	/**
	 * @brief Retrieve the Miss Combo at the given beat.
	 * @param fBeat the beat in question.
	 * @return the Miss Combo.
	 */
	int GetMissComboAtBeat( float fBeat ) const { return GetMissComboAtRow( BeatToNoteRow(fBeat) ); }
	/**
	 * @brief Set the row to have the new Combo.
	 * @param iNoteRow the row to have the new Combo.
	 * @param iCombo the Combo.
	 */
	void SetComboAtRow( int iNoteRow, int iCombo );
	/**
	 * @brief Set the beat to have the new Combo.
	 * @param fBeat the beat to have the new Combo.
	 * @param iCombo the Combo.
	 */
	void SetComboAtBeat( float fBeat, int iCombo ) { SetComboAtRow( BeatToNoteRow( fBeat ), iCombo ); }
	/**
	 * @brief Set the row to have the new Combo and Miss Combo.
	 * @param iNoteRow the row to have the new Combo and Miss Combo.
	 * @param iCombo the Combo.
	 * @param iMiss the Miss Combo.
	 */
	void SetComboAtRow( int iNoteRow, int iCombo, int iMiss );
	/**
	 * @brief Set the beat to have the new Combo and Miss Combo.
	 * @param fBeat the beat to have the new Combo and Miss Combo.
	 * @param iCombo the Combo.
	 * @param iMiss the Miss Combo.
	 */
	void SetComboAtBeat( float fBeat, int iCombo, int iMiss ) { SetComboAtRow( BeatToNoteRow( fBeat ), iCombo, iMiss ); }
	/**
	 * @brief Set the row to have the new Combo.
	 * @param iNoteRow the row to have the new Combo.
	 * @param iCombo the Combo.
	 */
	void SetHitComboAtRow( int iNoteRow, int iCombo );
	/**
	 * @brief Set the beat to have the new Combo.
	 * @param fBeat the beat to have the new Combo.
	 * @param iCombo the Combo.
	 */
	void SetHitComboAtBeat( float fBeat, int iCombo ) { SetHitComboAtRow( BeatToNoteRow( fBeat ), iCombo ); }
	/**
	 * @brief Set the row to have the new Miss Combo.
	 * @param iNoteRow the row to have the new Miss Combo.
	 * @param iCombo the Miss Combo.
	 */
	void SetMissComboAtRow( int iNoteRow, int iCombo );
	/**
	 * @brief Set the beat to have the new Miss Combo.
	 * @param fBeat the beat to have the new Miss Combo.
	 * @param iCombo the Miss Combo.
	 */
	void SetMissComboAtBeat( float fBeat, int iCombo ) { SetMissComboAtRow( BeatToNoteRow( fBeat ), iCombo ); }

	/**
	 * @brief Retrieve the Label at the given row.
	 * @param iNoteRow the row in question.
	 * @return the Label.
	 */
	RString GetLabelAtRow( int iNoteRow ) const;
	/**
	 * @brief Retrieve the Label at the given beat.
	 * @param fBeat the beat in question.
	 * @return the Label.
	 */
	RString GetLabelAtBeat( float fBeat ) const { return GetLabelAtRow( BeatToNoteRow(fBeat) ); }
	/**
	 * @brief Set the row to have the new Label.
	 * @param iNoteRow the row to have the new Label.
	 * @param sLabel the Label.
	 */
	void SetLabelAtRow( int iNoteRow, const RString sLabel );
	/**
	 * @brief Set the beat to have the new Label.
	 * @param fBeat the beat to have the new Label.
	 * @param sLabel the Label.
	 */
	void SetLabelAtBeat( float fBeat, const RString sLabel ) { SetLabelAtRow( BeatToNoteRow( fBeat ), sLabel ); }

	/**
	 * @brief Determine if the requisite label already exists.
	 * @param sLabel the label to check.
	 * @return true if it exists, false otherwise. */
	bool DoesLabelExist( RString sLabel ) const;
	/**
	 * @brief Retrieve the Speed's percent at the given row.
	 * @param iNoteRow the row in question.
	 * @return the percent.
	 */
	float GetSpeedPercentAtRow( int iNoteRow );
	/**
	 * @brief Retrieve the Speed's percent at the given beat.
	 * @param fBeat the beat in question.
	 * @return the percent.
	 */
	float GetSpeedPercentAtBeat( float fBeat ) { return GetSpeedPercentAtRow( BeatToNoteRow(fBeat) ); }
	/**
	 * @brief Retrieve the Speed's wait at the given row.
	 * @param iNoteRow the row in question.
	 * @return the wait.
	 */
	float GetSpeedWaitAtRow( int iNoteRow );
 	/**
	 * @brief Retrieve the Speed's wait at the given beat.
	 * @param fBeat the beat in question.
	 * @return the wait.
	 */
	float GetSpeedWaitAtBeat( float fBeat ) { return GetSpeedWaitAtRow( BeatToNoteRow(fBeat) ); }
	/**
	 * @brief Retrieve the Speed's mode at the given row.
	 * @param iNoteRow the row in question.
	 * @return the mode.
	 */
	SpeedSegment::BaseUnit GetSpeedModeAtRow( int iNoteRow );
 	/**
	 * @brief Retrieve the Speed's mode at the given beat.
	 * @param fBeat the beat in question.
	 * @return the mode.
	 */
	SpeedSegment::BaseUnit GetSpeedModeAtBeat( float fBeat ) { return GetSpeedModeAtRow( BeatToNoteRow(fBeat) ); }
	/**
	 * @brief Set the row to have the new Speed.
	 * @param iNoteRow the row to have the new Speed.
	 * @param fPercent the percent.
	 * @param fWait the wait.
	 * @param usMode the mode.
	 */
	void SetSpeedAtRow( int iNoteRow, float fPercent, float fWait, SpeedSegment::BaseUnit unit );
	/**
	 * @brief Set the beat to have the new Speed.
	 * @param fBeat the beat to have the new Speed.
	 * @param fPercent the percent.
	 * @param fWait the wait.
	 * @param usMode the mode.
	 */
	void SetSpeedAtBeat( float fBeat, float fPercent, float fWait, SpeedSegment::BaseUnit unit ) { SetSpeedAtRow( BeatToNoteRow(fBeat), fPercent, fWait, unit ); }
	/**
	 * @brief Set the row to have the new Speed percent.
	 * @param iNoteRow the row to have the new Speed percent.
	 * @param fPercent the percent.
	 */
	void SetSpeedPercentAtRow( int iNoteRow, float fPercent );
	/**
	 * @brief Set the beat to have the new Speed percent.
	 * @param fBeat the beat to have the new Speed percent.
	 * @param fPercent the percent.
	 */
	void SetSpeedPercentAtBeat( float fBeat, float fPercent ) { SetSpeedPercentAtRow( BeatToNoteRow(fBeat), fPercent); }
	/**
	 * @brief Set the row to have the new Speed wait.
	 * @param iNoteRow the row to have the new Speed wait.
	 * @param fWait the wait.
	 */
	void SetSpeedWaitAtRow( int iNoteRow, float fWait );
	/**
	 * @brief Set the beat to have the new Speed wait.
	 * @param fBeat the beat to have the new Speed wait.
	 * @param fWait the wait.
	 */
	void SetSpeedWaitAtBeat( float fBeat, float fWait ) { SetSpeedWaitAtRow( BeatToNoteRow(fBeat), fWait); }
	/**
	 * @brief Set the row to have the new Speed mode.
	 * @param iNoteRow the row to have the new Speed mode.
	 * @param usMode the mode.
	 */
	void SetSpeedModeAtRow( int iNoteRow, SpeedSegment::BaseUnit unit );
	/**
	 * @brief Set the beat to have the new Speed mode.
	 * @param fBeat the beat to have the new Speed mode.
	 * @param usMode the mode.
	 */
	void SetSpeedModeAtBeat( float fBeat, SpeedSegment::BaseUnit unit ) { SetSpeedModeAtRow( BeatToNoteRow(fBeat), unit); }

	float GetDisplayedSpeedPercent( float fBeat, float fMusicSeconds ) const;

	/**
	 * @brief Retrieve the scrolling factor at the given row.
	 * @param iNoteRow the row in question.
	 * @return the percent.
	 */
	float GetScrollAtRow( int iNoteRow );
	/**
	 * @brief Retrieve the scrolling factor at the given beat.
	 * @param fBeat the beat in question.
	 * @return the percent.
	 */
	float GetScrollAtBeat( float fBeat ) { return GetScrollAtRow( BeatToNoteRow(fBeat) ); }

	/**
	 * @brief Set the row to have the new Scrolling factor.
	 * @param iNoteRow the row to have the new Speed.
	 * @param fPercent the scrolling factor.
	 */
	void SetScrollAtRow( int iNoteRow, float fPercent );
	/**
	 * @brief Set the row to have the new Scrolling factor.
	 * @param iNoteRow the row to have the new Speed.
	 * @param fPercent the scrolling factor.
	 */
	void SetScrollAtBeat( float fBeat, float fPercent ) { SetScrollAtRow( BeatToNoteRow(fBeat), fPercent ); }

	/**
	 * @brief Determine when the fakes end.
	 * @param iRow The row you start on.
	 * @return the time when the fakes end.
	 */
	float GetFakeAtRow( int iRow ) const;
	/**
	 * @brief Determine when the fakes end.
	 * @param fBeat The beat you start on.
	 * @return the time when the fakes end.
	 */
	float GetFakeAtBeat( float fBeat ) const { return GetFakeAtRow( BeatToNoteRow( fBeat ) ); }
	/**
	 * @brief Set the beat to indicate when the FakeSegment ends.
	 * @param iRow The row to start on.
	 * @param fNew The destination beat.
	 */
	void SetFakeAtRow( int iRow, float fNew );
	/**
	 * @brief Set the beat to indicate when the FakeSegment ends.
	 * @param fBeat The beat to start on.
	 * @param fNew The destination beat.
	 */
	void SetFakeAtBeat( float fBeat, float fNew ) { SetFakeAtRow( BeatToNoteRow( fBeat ), fNew ); }

	/**
	 * @brief Checks if the row is inside a fake.
	 * @param iRow the row to focus on.
	 * @return true if the row is inside a fake, false otherwise.
	 */
	bool IsFakeAtRow( int iRow ) const;
	/**
	 * @brief Checks if the beat is inside a fake.
	 * @param fBeat the beat to focus on.
	 * @return true if the row is inside a fake, false otherwise.
	 */
	bool IsFakeAtBeat( float fBeat ) const { return IsFakeAtRow( BeatToNoteRow( fBeat ) ); }
	
	/**
	 * @brief Determine if this notes on this row can be judged.
	 * @param row the row to focus on.
	 * @return true if the row can be judged, false otherwise. */
	bool IsJudgableAtRow( int row ) const
	{
		return !(IsWarpAtRow(row) || IsFakeAtRow(row));
	}
	/**
	 * @brief Determine if this notes on this beat can be judged.
	 * @param beat the beat to focus on.
	 * @return true if the row can be judged, false otherwise. */
	bool IsJudgableAtBeat( float beat ) const { return IsJudgableAtRow( BeatToNoteRow( beat ) ); }
	
	
	
	void MultiplyBPMInBeatRange( int iStartIndex, int iEndIndex, float fFactor );
	
	void NoteRowToMeasureAndBeat( int iNoteRow, int &iMeasureIndexOut, int &iBeatIndexOut, int &iRowsRemainder ) const;

	void GetBeatAndBPSFromElapsedTime( float fElapsedTime, float &fBeatOut, float &fBPSOut, bool &bFreezeOut, bool &bDelayOut, int &iWarpBeginOut, float &fWarpLengthOut ) const;
	float GetBeatFromElapsedTime( float fElapsedTime ) const	// shortcut for places that care only about the beat
	{
		float fBeat, fThrowAway, fThrowAway2;
		bool bThrowAway, bThrowAway2;
		int iThrowAway;
		GetBeatAndBPSFromElapsedTime( fElapsedTime, fBeat, fThrowAway, bThrowAway, bThrowAway2, iThrowAway, fThrowAway2 );
		return fBeat;
	}
	float GetElapsedTimeFromBeat( float fBeat ) const;

	void GetBeatAndBPSFromElapsedTimeNoOffset( float fElapsedTime, float &fBeatOut, float &fBPSOut, bool &bFreezeOut, bool &bDelayOut, int &iWarpBeginOut, float &fWarpDestinationOut ) const;
	float GetBeatFromElapsedTimeNoOffset( float fElapsedTime ) const	// shortcut for places that care only about the beat
	{
		float fBeat, fThrowAway, fThrowAway2;
		bool bThrowAway, bThrowAway2;
		int iThrowAway;
		GetBeatAndBPSFromElapsedTimeNoOffset( fElapsedTime, fBeat, fThrowAway, bThrowAway, bThrowAway2, iThrowAway, fThrowAway2 );
		return fBeat;
	}
	float GetElapsedTimeFromBeatNoOffset( float fBeat ) const;
	float GetDisplayedBeat( float fBeat ) const;
	/**
	 * @brief View the TimingData to see if a song changes its BPM at any point.
	 * @return true if there is at least one change, false otherwise.
	 */
	bool HasBpmChanges() const;
	/**
	 * @brief View the TimingData to see if there is at least one stop at any point.
	 * @return true if there is at least one stop, false otherwise.
	 */
	bool HasStops() const;
	/**
	 * @brief View the TimingData to see if there is at least one delay at any point.
	 * @return true if there is at least one delay, false otherwise.
	 */
	bool HasDelays() const;
	/**
	 * @brief View the TimingData to see if there is at least one warp at any point.
	 * @return true if there is at least one warp, false otherwise.
	 */
	bool HasWarps() const;
	/**
	 * @brief View the TimingData to see if there is at least one fake segment involved.
	 * @return true if there is at least one fake segment, false otherwise. */
	bool HasFakes() const;
	/**
	 * @brief View the TimingData to see if a song changes its speed scrolling at any point.
	 * @return true if there is at least one change, false otherwise. */
	bool HasSpeedChanges() const;
	/**
	 * @brief View the TimingData to see if a song changes its speed scrolling at any point.
	 * @return true if there is at least one change, false otherwise. */
	bool HasScrollChanges() const;
	/**
	 * @brief Compare two sets of timing data to see if they are equal.
	 * @param other the other TimingData.
	 * @return the equality or lack thereof of the two TimingData.
	 */
	bool operator==( const TimingData &other )
	{
		FOREACH_ENUM( TimingSegmentType, tst )
		{
			const vector<TimingSegment*> &us = m_avpTimingSegments[tst];
			const vector<TimingSegment*> &them = other.m_avpTimingSegments[tst];

			// optimization: check vector sizes before contents
			if( us.size() != them.size() )
				return false;

			if( !std::equal(us.begin(), us.end(), them.begin()) )
				return false;
		}

		COMPARE( m_fBeat0OffsetInSeconds );
		return true;
	}

	/**
	 * @brief Compare two sets of timing data to see if they are not equal.
	 * @param other the other TimingData.
	 * @return the inequality or lack thereof of the two TimingData.
	 */
	bool operator!=( const TimingData &other ) { return !operator==(other); }

	void ScaleRegion( float fScale = 1, int iStartRow = 0, int iEndRow = MAX_NOTE_ROW, bool bAdjustBPM = false );
	void InsertRows( int iStartRow, int iRowsToAdd );
	void DeleteRows( int iStartRow, int iRowsToDelete );

	/**
	 * @brief Tidy up the timing data, e.g. provide default BPMs, labels, tickcounts.
	 */
	void TidyUpData();

	// Lua
	void PushSelf( lua_State *L );
	/**
	 * @brief The file of the song/steps that use this TimingData.
	 *
	 * This is for informational purposes only.
	 */
	RString					m_sFile;

	// All of the following vectors must be sorted before gameplay.
	vector<TimingSegment *> m_avpTimingSegments[NUM_TimingSegmentType];

	/**
	 * @brief The initial offset of a song.
	 */
	float	m_fBeat0OffsetInSeconds;

	vector<RString> ToVectorString(TimingSegmentType tst, int dec = 6) const;
};

#undef COMPARE

#endif

/**
 * @file
 * @author Chris Danford, Glenn Maynard (c) 2001-2004 
 * @section LICENSE
 * All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, and/or sell copies of the Software, and to permit persons to
 * whom the Software is furnished to do so, provided that the above
 * copyright notice(s) and this permission notice appear in all copies of
 * the Software and that both the above copyright notice(s) and this
 * permission notice appear in supporting documentation.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF
 * THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS
 * INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
