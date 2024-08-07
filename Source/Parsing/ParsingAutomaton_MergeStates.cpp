/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#include "ParsingAutomaton.h"

namespace vl
{
	namespace parsing
	{
		using namespace collections;
		using namespace definitions;

		namespace analyzing
		{

/***********************************************************************
DeleteUnnecessaryStates
***********************************************************************/

			void DeleteUnnecessaryStates(Ptr<Automaton> automaton, Ptr<RuleInfo> ruleInfo, List<State*>& newStates)
			{
				// delete all states that are not reachable to the end state
				while(true)
				{
					// find a non-end state without out transitions
					vint deleteCount=0;
					for(vint i=newStates.Count()-1;i>=0;i--)
					{
						State* newState=newStates[i];
						if(newState->transitions.Count()==0)
						{
							if(newState!=ruleInfo->rootRuleEndState && !newState->endState)
							{
								automaton->DeleteState(newState);
								newStates.RemoveAt(i);
							}
						}
					}
					if(deleteCount==0)
					{
						break;
					}
				}
			}

/***********************************************************************
IsMergableCandidate
***********************************************************************/

			bool IsMergableCandidate(State* state, Ptr<RuleInfo> ruleInfo)
			{
				if(state==ruleInfo->rootRuleStartState || state==ruleInfo->rootRuleEndState || state==ruleInfo->startState)
				{
					return false;
				}
				return true;
			}

/***********************************************************************
RearrangeState
***********************************************************************/

			std::strong_ordering CompareParsingSymbol(ParsingSymbol* s1, ParsingSymbol* s2)
			{
				if (s1 && s2)
				{
					std::strong_ordering
						result = s1->GetType() <=> s2->GetType(); if (result != 0) return result;
					result = s1->GetName() <=> s2->GetName(); if (result != 0) return result;
					return result;
				}
				else if (s1)
				{
					return std::strong_ordering::greater;
				}
				else if (s2)
				{
					return std::strong_ordering::less;
				}
				else
				{
					return std::strong_ordering::equal;
				}
			}

			std::strong_ordering CompareTransitionForRearranging(Transition* t1, Transition* t2)
			{
				std::strong_ordering
				result = t1->transitionType <=> t2->transitionType; if (result != 0) return result;
				result = CompareParsingSymbol(t1->transitionSymbol, t2->transitionSymbol); if (result != 0) return result;
				return result;
			}

			std::strong_ordering CompareActionForRearranging(Ptr<Action> a1, Ptr<Action> a2)
			{
				std::strong_ordering
				result = a1->actionType <=> a2->actionType; if (result != 0) return result;
				result = CompareParsingSymbol(a1->actionSource, a2->actionSource); if (result != 0) return result;
				result = CompareParsingSymbol(a1->actionTarget, a2->actionTarget); if (result != 0) return result;
				return result;
			}

#undef COMPARE_SYMBOL

			void RearrangeState(State* state, SortedList<State*>& stateContentSorted)
			{
				if(!stateContentSorted.Contains(state))
				{
					for (auto transition : state->transitions)
					{
						CopyFrom(transition->actions, From(transition->actions).OrderBy(&CompareActionForRearranging));
					}
					CopyFrom(state->transitions, From(state->transitions).OrderBy(&CompareTransitionForRearranging));
					stateContentSorted.Add(state);
				}
			}

/***********************************************************************
MoveActionsForMergingState
***********************************************************************/

			void MoveActionsForMergingState(Transition* transition)
			{
				// collect all movable actions
				List<Ptr<Action>> movableActions;
				for(vint i=transition->actions.Count()-1;i>=0;i--)
				{
					switch(transition->actions[i]->actionType)
					{
					// Using and Assign actions are not movable
					case Action::Using:
					case Action::Assign:
						break;
					default:
						movableActions.Add(transition->actions[i]);
						transition->actions.RemoveAt(i);
					}
				}

				// copy all movable actions
				for (auto t : transition->source->inputs)
				{
					CopyFrom(t->actions, movableActions, true);
				}
			}

/***********************************************************************
IsMergableBecause(Transitions|Input)
***********************************************************************/

			bool IsMergableBecauseTransitions(State* state1, State* state2)
			{
				if(state1->transitions.Count()!=state2->transitions.Count()) return false;
				if(state1->transitions.Count()==1 && state2->transitions.Count()==1)
				{
					Transition* t1=state1->transitions[0];
					Transition* t2=state2->transitions[0];
					if(CompareTransitionForRearranging(t1, t2)==0 && !Transition::IsEquivalent(t1, t2, false) && t1->target==t2->target)
					{
						MoveActionsForMergingState(t1);
						MoveActionsForMergingState(t2);
					}
				}
				for(vint i=0;i<state1->transitions.Count();i++)
				{
					Transition* t1=state1->transitions[i];
					Transition* t2=state2->transitions[i];
					if(!Transition::IsEquivalent(t1, t2, false) || t1->target!=t2->target)
					{
						return false;
					}
				}
				return true;
			}

			bool IsMergableBecauseInputs(State* state1, State* state2)
			{
				if(state1->inputs.Count()!=state2->inputs.Count()) return false;
				for(vint i=0;i<state1->inputs.Count();i++)
				{
					Transition* t1=state1->inputs[i];
					Transition* t2=state2->inputs[i];
					if(!Transition::IsEquivalent(t1, t2, false) || t1->source!=t2->source)
					{
						return false;
					}
				}
				return true;
			}

/***********************************************************************
MergeState2ToState1Because(Transitions|Input)
***********************************************************************/

			void MergeState2ToState1BecauseTransitions(Ptr<Automaton> automaton, State* state1, State* state2)
			{
				// modify state1's expression
				state1->stateExpression+=L"\r\n"+state2->stateExpression;

				// retarget state2's input to state1
				for(vint i=state2->inputs.Count()-1;i>=0;i--)
				{
					Transition* t2=state2->inputs[i];
					bool add=true;
					for (auto t1 : state1->inputs)
					{
						if(Transition::IsEquivalent(t1, t2, false) && t1->source==t2->source)
						{
							add=false;
							break;
						}
					}
					if(add)
					{
						state1->inputs.Add(t2);
						t2->target=state1;
						state2->inputs.RemoveAt(i);
					}
				}

				automaton->DeleteState(state2);
			}

			void MergeState2ToState1BecauseInputs(Ptr<Automaton> automaton, State* state1, State* state2)
			{
				// modify state1's expression
				state1->stateExpression+=L"\r\n"+state2->stateExpression;

				// retarget state2's input to state1
				for(vint i=state2->transitions.Count()-1;i>=0;i--)
				{
					Transition* t2=state2->transitions[i];
					bool add=true;
					for (auto t1 : state1->transitions)
					{
						if(Transition::IsEquivalent(t1, t2, false) && t1->target==t2->target)
						{
							add=false;
							break;
						}
					}
					if(add)
					{
						state1->transitions.Add(t2);
						t2->source=state1;
						state2->transitions.RemoveAt(i);
					}
				}

				automaton->DeleteState(state2);
			}

/***********************************************************************
MergeStates
***********************************************************************/

			void MergeStates(Ptr<Automaton> automaton, Ptr<RuleInfo> ruleInfo, List<State*>& newStates)
			{
				SortedList<State*> stateContentSorted;
				while(true)
				{
					for(vint i=0;i<newStates.Count();i++)
					{
						State* state1=newStates[i];
						if(IsMergableCandidate(state1, ruleInfo))
						{
							for(vint j=i+1;j<newStates.Count();j++)
							{
								State* state2=newStates[j];
								if(state1!=state2 && IsMergableCandidate(state2, ruleInfo))
								{
									RearrangeState(state1, stateContentSorted);
									RearrangeState(state2, stateContentSorted);
									if(IsMergableBecauseTransitions(state1, state2))
									{
										MergeState2ToState1BecauseTransitions(automaton, state1, state2);
										newStates.RemoveAt(j);
										goto MERGED_STATES_PAIR;
									}
									else if(IsMergableBecauseInputs(state1, state2))
									{
										MergeState2ToState1BecauseInputs(automaton, state1, state2);
										newStates.RemoveAt(j);
										goto MERGED_STATES_PAIR;
									}
								}
							}
						}
					}
					break;
				MERGED_STATES_PAIR:
					continue;
				}
			}
		}
	}
}